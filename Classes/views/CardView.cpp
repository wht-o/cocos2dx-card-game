#include "CardView.h"
#include "utils/CardResUtils.h"

using namespace cocos2d;

CardView::~CardView() {
    if (_cardModel) {
        _cardModel->release(); // 释放init中retain的CardModel
        _cardModel = nullptr;
    }
    // 可选：清理触摸监听器（避免悬空指针）
    if (_touchListener) {
        _eventDispatcher->removeEventListener(_touchListener);
        _touchListener = nullptr;
    }
}

CardView* CardView::create(CardModel* cardModel) {
    auto view = new CardView();
    if (view && view->_init(cardModel)) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool CardView::_init(CardModel* cardModel) {
    if (!Layer::init()) {
        return false;
    }
    _cardModel = cardModel;
    if (_cardModel) {
        _cardModel->retain(); // 防止 CardModel 被自动释放
        CCLOG("CardView::init - Associated with CardModel: %p (ref count: %d)",
            _cardModel, _cardModel->getReferenceCount());
    }
    else {
        CCLOGERROR("CardView::init - CardModel is null!");
        return false;
    }
    _initUI();
    _updateCardOpenState();
    // 初始化时更新翻开状态
    _updateCardOpenState();

    // 注册触摸事件
    _touchListener = EventListenerTouchOneByOne::create();
    _touchListener->onTouchBegan = CC_CALLBACK_2(CardView::onTouchBegan, this);
    _touchListener->setSwallowTouches(true); // 吞噬触摸，避免透传
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

    return true;
}

void CardView::_initUI() {
    if (!_cardModel) {
        CCLOG("CardView::_initUI - CardModel is null!");
        return;
    }
    this->setAnchorPoint(Vec2(0.5f, 0.5f));

    // 卡牌基础尺寸（182×282，可根据实际资源调整）
    const Size CARD_SIZE = Size(182.0f, 282.0f);

    // 创建卡牌背面（覆盖状态显示）
    _cardBack = Sprite::create("res/card_back.png");
    if (!_cardBack) {
        CCLOG("CardView::_initUI - Load card_back.png failed!");
        return;
    }
    _cardBack->setAnchorPoint(Vec2(0.5f, 0.5f)); // 锚点居中
    _cardBack->setPosition(Vec2::ZERO); // 子节点相对父节点居中
    this->addChild(_cardBack);

    //创建卡牌正面背景
    _cardBg = Sprite::create("res/card_general.png");
    if (!_cardBg) {
        CCLOG("CardView::_initUI - Load card_general.png failed!");
        return;
    }
    _cardBg->setAnchorPoint(Vec2(0.5f, 0.5f));
    _cardBg->setPosition(Vec2::ZERO);
    _cardBg->setVisible(false); // 默认隐藏
    this->addChild(_cardBg);

    // 创建花色（先创建再设置属性，修正空指针）
    CardSuitType suit = _cardModel->getSuit();
    std::string suitPath = CardResUtils::getSuitResPath(suit);
    _suit = Sprite::create(suitPath);
    if (_suit) {
        _suit->setAnchorPoint(Vec2(1.0f, 1.0f));
        _suit->setPosition(Vec2(80, 120)); // 右上角（基于182×282尺寸）
        _suit->setVisible(false);
        this->addChild(_suit);
    }

    //数字颜色（红/黑）
    bool isRed = CardResUtils::isSuitRed(suit);
    CardFaceType face = _cardModel->getFace();

    //大号数字（居中）
    std::string bigNumPath = CardResUtils::getBigNumberResPath(face, isRed);
    _bigNumber = Sprite::create(bigNumPath);
    if (_bigNumber) {
        _bigNumber->setAnchorPoint(Vec2(0.5f, 0.5f));
        _bigNumber->setPosition(Vec2(0,-40)); // 卡牌中心
        _bigNumber->setVisible(false);
        this->addChild(_bigNumber);
    }

    //小号数字（左上角）
    std::string smallNumPath = CardResUtils::getSmallNumberResPath(face, isRed);
    _smallNumber = Sprite::create(smallNumPath);
    if (_smallNumber) {
        _smallNumber->setAnchorPoint(Vec2(0.0f, 1.0f));
        _smallNumber->setPosition(Vec2(-80, +120)); // 左上角
        _smallNumber->setVisible(false);
        this->addChild(_smallNumber);
    }

    // 只设置CardView本身的位置（子节点相对父节点居中）
    this->setPosition(_cardModel->getPosition());
    // 设置CardView的内容尺寸（方便触摸检测）
    this->setContentSize(CARD_SIZE);
}

bool CardView::onTouchBegan(Touch* touch, Event* event) {
    if (!_cardModel || !_clickCallback) {
        return false;
    }

    Vec2 touchWorldPos = touch->getLocation();
    Vec2 touchLocalPos = this->convertToNodeSpace(touchWorldPos);
    Rect cardRect = Rect(
        -this->getContentSize().width / 2,
        -this->getContentSize().height / 2,
        this->getContentSize().width,
        this->getContentSize().height
    );

    if (cardRect.containsPoint(touchLocalPos)) {
        if (_cardModel->isCardOpen()) {
            int cardId = _cardModel->getCardId();
            // 过滤非法cardId（负数/异常值）
            if (cardId >= 0) {
                _clickCallback(cardId);
            }
            else {
                CCLOG("CardView::onTouchBegan - Invalid cardId: %d (cardModel: %p)", cardId, _cardModel);
                return true; // 吞掉事件但不触发回调
            }
        }
        return true;
    }
    return false;
}

void CardView::moveToPosition(const Vec2& targetPos, float duration) {
    if (duration <= 0) {
        this->setPosition(targetPos); // 无动画直接移动
    }
    else {
        auto moveAction = MoveTo::create(duration, targetPos);
        this->runAction(moveAction);
    }

    // 同步更新模型位置（注意：模型仅存储数据，视图移动后同步）
    if (_cardModel) {
        _cardModel->setPosition(targetPos);
    }
}

void CardView::_updateCardOpenState() {
    if (!_cardModel || !_cardBack || !_cardBg) {
        CCLOG("CardView::_updateCardOpenState - Missing UI components!");
        return;
    }

    bool isOpen = _cardModel->isCardOpen();
    // 切换正反面显示
    _cardBack->setVisible(!isOpen);
    _cardBg->setVisible(isOpen);
    if (_suit) _suit->setVisible(isOpen);
    if (_bigNumber) _bigNumber->setVisible(isOpen);
    if (_smallNumber) _smallNumber->setVisible(isOpen);

    // 控制触摸监听的启用/禁用
    if (_touchListener) {
        _touchListener->setEnabled(isOpen);
    }
}

void CardView::refreshOpenState() {
    _updateCardOpenState();
}