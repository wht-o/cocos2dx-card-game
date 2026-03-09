#include "GameView.h"
#include "cocos2d.h"

using namespace cocos2d;


GameView* GameView::create(GameModel* gameModel) {
    auto view = new GameView();
    if (view && view->_init(gameModel)) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}


bool GameView::_init(GameModel* gameModel) {
    if (!Layer::init()) {
        return false;
    }
    _gameModel = gameModel;
    _initPlayfieldCards();    // 初始化主牌区卡牌
    _initStackCards();        // 初始化底牌堆卡牌
    _initUndoButton();        // 初始化回退按钮UI
    return true;
}


void GameView::_cleanInvalidCardViews() {
    for (auto it = _cardViews.begin(); it != _cardViews.end();) {
        int cardId = it->first;
        CardView* cardView = it->second;

        // 判断卡牌视图是否无效：空指针 或 未在场景运行
        bool isInvalid = !cardView || !cardView->isRunning();
        if (isInvalid) {
            it = _cardViews.erase(it); // 擦除并获取下一个迭代器（避免迭代器失效）
            CCLOG("GameView::cleanInvalidCardViews - 移除无效卡牌视图，ID：%d", cardId);
        }
        else {
            ++it;
        }
    }
}


void GameView::_initPlayfieldCards() {
    if (!_gameModel) {
        CCLOG("GameView::initPlayfieldCards - GameModel为空！");
        return;
    }

    const auto& playfieldCards = _gameModel->getPlayfieldCards();
    for (auto cardModel : playfieldCards) {
        if (!cardModel) {
            CCLOG("GameView::initPlayfieldCards - 无效的CardModel！");
            continue;
        }

        auto cardView = CardView::create(cardModel);
        if (!cardView) {
            CCLOG("GameView::initPlayfieldCards - 创建CardView失败！");
            continue;
        }

        // 绑定卡牌点击回调（调用前检查GameView有效性）
        cardView->setCardClickCallback([this](int cardId) {
            if (!_isGameViewValid()) {
                CCLOG("GameView::cardClick - GameView无效，跳过回调！");
                return;
            }
            if (_cardClickCallback) {
                _cardClickCallback(cardId);
            }
            });

        // 计算卡牌绝对位置（添加边界检查，避免超出屏幕）
        Vec2 cardRelativePos = cardModel->getPosition();
        cardView->setPosition(cardRelativePos);

        // 刷新卡牌翻开/关闭状态
        cardView->refreshOpenState();

        // 设置卡牌ID为Tag（便于后续快速查找）
        int cardId = cardModel->getCardId();
        cardView->setTag(cardId);
        this->addChild(cardView);
        _cardViews[cardId] = cardView; // 添加到卡牌视图映射表
    }
}


void GameView::_initStackCards() {
    if (!_gameModel) {
        CCLOG("GameView::initStackCards - GameModel为空！");
        return;
    }

    const auto& stackCards = _gameModel->getStackCards();
    for (auto cardModel : stackCards) {
        if (!cardModel) {
            CCLOG("GameView::initStackCards - 无效的CardModel！");
            continue;
        }

        auto cardView = CardView::create(cardModel);
        if (!cardView) {
            CCLOG("GameView::initStackCards - 创建CardView失败！");
            continue;
        }

        // 绑定卡牌点击回调（与主牌区卡牌共用有效性检查逻辑）
        cardView->setCardClickCallback([this](int cardId) {
            if (!_isGameViewValid()) {
                CCLOG("GameView::cardClick - GameView无效，跳过回调！");
                return;
            }
            if (_cardClickCallback) {
                _cardClickCallback(cardId);
            }
            });

        // 计算底牌堆卡牌位置
        Vec2 cardStackPos = cardModel->getPosition();

        cardView->setPosition(cardStackPos);

        // 刷新卡牌翻开/关闭状态
        cardView->refreshOpenState();

        // 设置Tag并添加到场景和映射表
        int cardId = cardModel->getCardId();
        cardView->setTag(cardId);
        this->addChild(cardView);
        _cardViews[cardId] = cardView;
    }
}


void GameView::_initUndoButton() {
    _undoBtn = MenuItemImage::create(
        "res/undo_btn_normal.png",  // 常态纹理
        "res/undo_btn_pressed.png", // 按下态纹理
        CC_CALLBACK_0(GameView::_onUndoBtnClicked, this) // 点击回调
    );
    if (!_undoBtn) {
        CCLOG("GameView::initUndoButton - 创建回退按钮失败！");
        return;
    }

    // 设置回退按钮位置（固定在屏幕右下角）
    const float UNDO_BTN_POS_X = 1000.0f;
    const float UNDO_BTN_POS_Y = 200.0f;
    _undoBtn->setPosition(UNDO_BTN_POS_X, UNDO_BTN_POS_Y);

    // 创建菜单并添加到层
    auto menu = Menu::create(_undoBtn, nullptr);
    menu->setPosition(Vec2::ZERO); // 菜单位置为原点（按钮位置相对菜单）
    this->addChild(menu);
}


void GameView::_onUndoBtnClicked() {
    if (!_isGameViewValid()) {
        CCLOG("GameView::onUndoBtnClicked - GameView无效，跳过回调！");
        return;
    }
    if (_undoClickCallback) {
        _undoClickCallback();
    }
}

void GameView::moveCardToPosition(int cardId, const Vec2& targetPos) {
    _cleanInvalidCardViews();

    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end()) {
        CardView* cardView = it->second;
        if (cardView && cardView->getParent()) {
            // 将卡牌zOrder设为父节点子节点的最大值+1
            int maxZOrder = 0;
            for (const auto& child : cardView->getParent()->getChildren()) {
                if (child->getLocalZOrder() > maxZOrder) {
                    maxZOrder = child->getLocalZOrder();
                }
            }
            cardView->setLocalZOrder(maxZOrder + 1);

            // 执行卡牌移动
            cardView->moveToPosition(targetPos);
            CCLOG("GameView::moveCardToPosition - 卡牌[%d]已移动到位置(%.2f,%.2f)并置顶显示",
                cardId, targetPos.x, targetPos.y);
        }
    }
    else {
        CCLOG("GameView::moveCardToPosition - 未找到卡牌视图，ID：%d", cardId);
    }
}


void GameView::updateStackTopCard(CardModel* newCardModel) {

    // 清理旧的顶部卡牌视图（确保内存安全）
    if (_stackTopCardView) {
        _stackTopCardView->removeFromParent(); // 从场景树移除
        _stackTopCardView = nullptr;           // 指针置空
    }

    // 模型有效时创建新的顶部卡牌视图
    if (newCardModel) {
        _stackTopCardView = CardView::create(newCardModel);
        if (_stackTopCardView) {
            // 设置顶部卡牌位置（添加边界检查，避免超出屏幕）
            const float STACK_CARD_POS_X = 750.0f;
            const float STACK_CARD_POS_Y = 100.0f;
            _stackTopCardView->setPosition(
                clampf(STACK_CARD_POS_X, 0.0f, 1080.0f), // X轴限制在屏幕宽度内
                clampf(STACK_CARD_POS_Y, 0.0f, 720.0f)   // Y轴限制在屏幕高度内
            );
            this->addChild(_stackTopCardView);
        }
    }
}

void GameView::refreshCardOpenState(int cardId) {
    // 先清理无效视图
    _cleanInvalidCardViews();

    // 检查卡牌视图映射表是否为空
    if (_cardViews.empty()) {
        CCLOG("GameView::refreshCardOpenState - 卡牌视图映射表为空！");
        return;
    }

    // 根据ID查找卡牌视图
    auto it = _cardViews.find(cardId);
    if (it == _cardViews.end()) {
        CCLOG("GameView::refreshCardOpenState - 未找到卡牌视图，ID：%d", cardId);
        return;
    }

    // 视图有效时刷新状态
    CardView* cardView = it->second;
    if (cardView) {
        cardView->refreshOpenState(); // 更新卡牌视图的翻开/关闭状态
    }
    else {
        CCLOG("GameView::refreshCardOpenState - 卡牌视图无效，ID：%d", cardId);
        _cardViews.erase(it); // 从映射表移除无效指针
    }
}

void GameView::removeCardView(int cardId) {
    _cleanInvalidCardViews(); // 先清理无效视图，避免迭代器失效
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end()) {
        CardView* cardView = it->second;
        if (cardView) {
            cardView->removeFromParent(); // 从场景树移除
        }
        _cardViews.erase(it); // 从映射表删除
        CCLOG("GameView::removeCardView ，ID：%d", cardId);
    }
    else {
        CCLOG("GameView::removeCardView ，ID：%d", cardId);
    }
}