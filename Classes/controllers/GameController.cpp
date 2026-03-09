#include "GameController.h"

using namespace cocos2d;



GameController* GameController::create(int levelId) {
    // 空值校验：关卡ID非法时直接返回（避免无效初始化）
    if (levelId < 1) {
        CCLOG("GameController::create - Invalid levelId: %d (must >=1)", levelId);
        return nullptr;
    }

    // 标准Cocos2d-x对象创建流程
    auto controller = new (std::nothrow) GameController(); // 用nothrow避免new失败抛异常
    if (controller) {
        // 初始化成功则设置自动释放，否则销毁对象
        if (controller->_init(levelId)) {
            controller->autorelease(); // 加入自动释放池，避免内存泄漏
            CCLOG("GameController::create - Create success, levelId: %d", levelId);
            return controller;
        }
        else {
            CCLOG("GameController::create - Init failed, levelId: %d", levelId);
            CC_SAFE_DELETE(controller); // 初始化失败，手动释放
        }
    }
    else {
        CCLOG("GameController::create - New controller failed (out of memory?)");
    }

    return nullptr;
}

bool GameController::_init(int levelId) {
    _levelId = levelId;
    _gameModel = nullptr; // 初始化为空，由startGame创建
    _undoModel = UndoModel::create();
    if (_undoModel) {
        _undoModel->retain();
    }
    return true;
}


void GameController::startGame() {
    // 加载关卡配置
    LevelConfig levelConfig = LevelConfigLoader::loadLevelConfig(_levelId);

    // 释放旧的GameModel（避免内存泄漏+野指针）
    if (_gameModel) {
        _gameModel->release();
        _gameModel = nullptr;
    }

    // 创建新的 GameModel
    _gameModel = GameModelGenerator::generateGameModel(levelConfig);
    if (_gameModel) {
        _gameModel->retain();  // 手动持有，抵消autorelease()
        CCLOG("GameController::startGame - Successfully created GameModel | Number of cards in game area: %zu | Number of cards in stack area: %zu",
            _gameModel->getPlayfieldCards().size(),
            _gameModel->getStackCards().size());
    }
    else {
        CCLOGERROR("GameController::startGame - GameModelGenerator returned null! Creating empty GameModel for fault tolerance");
        _gameModel = GameModel::create();
        if (_gameModel) {
            _gameModel->retain();  // 手动持有
        }
    }

    // 释放旧的UndoManager（关键：避免野指针）
    if (_undoManager) {
        delete _undoManager;
        _undoManager = nullptr;
    }

    // 释放旧的GameView（避免内存泄漏）
    if (_gameView) {
        _gameView->release();
        _gameView = nullptr;
    }

   
    // 初始化GameView并手动持有
    _gameView = GameView::create(_gameModel);
    if (_gameView) {
        _gameView->retain(); // 持有GameView，防止被自动释放
    }

    
    _gameView->setCardClickCallback([this](int cardId) {
        this->handleCardClick(cardId);
        });
   
    _gameView->setUndoClickCallback([this]() {
        this->handleUndoClick();
        });
   

    // 重新创建UndoManager
    _undoManager = new UndoManager(_undoModel, _gameModel, _gameView);
  

    auto director = Director::getInstance();
  
    auto scene = Scene::create();
    
    scene->addChild(_gameView); // GameView被scene持有，引用计数+1
  
    director->runWithScene(scene);
}

// 析构函数：释放持有的 Ref 实例
GameController::~GameController() {
    // 释放 GameModel
    if (_gameModel) {
        _gameModel->release();
        _gameModel = nullptr;
    }
    // 释放 UndoModel
    if (_undoModel) {
        _undoModel->release();
        _undoModel = nullptr;
    }
    // 释放 GameView
    if (_gameView) {
        _gameView->release();
        _gameView = nullptr;
    }
    // 释放 UndoManager
    if (_undoManager) {
        delete _undoManager;
        _undoManager = nullptr;
    }
}


void GameController::openUnderCard(int cardId) {
    // 这里需根据你的“牌堆层级规则”实现（比如按位置/y坐标判断上下层级）
    // 示例逻辑：找到y坐标略小、同列的第一张覆盖卡牌，将其翻开
    const auto& playfieldCards = _gameModel->getPlayfieldCards();
    CardModel* underCard = nullptr;
    auto targetCard = _gameModel->getPlayfieldCardById(cardId);
    if (!targetCard) return;

    float targetY = targetCard->getPosition().y;
    float targetX = targetCard->getPosition().x;
    for (auto card : playfieldCards) {
        // 下方牌：y坐标更小，x坐标接近（同列），且未翻开
        if (card->getPosition().y < targetY
            && abs(card->getPosition().x - targetX) < 50
            && !card->isCardOpen()) {
            underCard = card;
            break;
        }
    }

    // 翻开下方牌：更新模型 + 通知视图刷新
    if (underCard) {
        underCard->setCardOpen(true);
        _gameView->refreshCardOpenState(underCard->getCardId()); // 视图层刷新显示
    }
}

bool GameController::handleCardClick(int cardId) {

  
    // 核心对象空指针检查（优先拦截无效调用）
    if (!_gameModel || !_gameView) {
        CCLOGERROR("GameController::handleCardClick - _gameModel(%p) or _gameView(%p) is null!", _gameModel, _gameView);
        return false;
    }
    
   
    auto playfieldCard = _gameModel->getPlayfieldCardById(cardId);
    auto stackTopCard = _gameModel->getStackTopCard();

    // 场景1：点击的不是游戏区卡牌
    if (!playfieldCard) {
        _handleStackReplace(cardId);
        
        return true;
    }
 
    // 场景2：游戏区卡牌匹配栈顶卡牌
    if (CardMatchService::isCardMatch(stackTopCard, playfieldCard)) {
        _handlePlayfieldMatch(cardId);
      
        return true;
    }

    return false;
}

void GameController::_handleStackReplace(int cardId) {
  
    // 获取原始数据
    auto originalTopCard = _gameModel->getStackTopCard();
    auto newCard = _gameModel->getStackCardById(cardId);
    if (!originalTopCard || !newCard) {
        return;
    }
    Vec2 originalPos = newCard->getPosition();

    
    // 记录回退操作
    UndoRecord record;
    record.opType = UOT_STACK_REPLACE;
    record.cardId = cardId;
    record.originalCard = originalTopCard;
    record.operatedCard = newCard;
    record.originalPosition = originalPos;
    _undoManager->addUndoRecord(record);

    // 更新模型
    _gameModel->removePlayfieldCardById(cardId);
    _gameModel->replaceStackTopCard(newCard);

    // 更新视图（移动卡牌到底牌堆位置）
    _gameView->moveCardToPosition(cardId, Vec2(750, 400));


}

void GameController::_handlePlayfieldMatch(int cardId) {
    // 获取原始数据
    auto originalTopCard = _gameModel->getStackTopCard();
    auto matchCard = _gameModel->getPlayfieldCardById(cardId);
    if (!originalTopCard || !matchCard) {
        return;
    }
    Vec2 originalPos = matchCard->getPosition();

 
   
    // 记录回退操作
    UndoRecord record;
    record.opType = UOT_PLAYFIELD_MATCH;
    record.cardId = cardId;
    record.originalCard = originalTopCard;
    record.operatedCard = matchCard;
    record.originalPosition = originalPos ;
    _undoManager->addUndoRecord(record);


   
    // 更新模型
    _gameModel->removePlayfieldCardById(cardId);
    _gameModel->replaceStackTopCard(matchCard);

    // 更新视图（移动卡牌到底牌堆位置）
    _gameView->moveCardToPosition(cardId, Vec2(750,400));
    
  ;
}

void GameController::handleUndoClick() {
    CCLOG("=== GameController::handleUndoClick ===");
    CCLOG("_undoManager: %p", _undoManager);

    // 双重校验：非空 + 非释放标记（0xDDDDDDDD是VC++调试模式释放标记）
    const uintptr_t INVALID_PTR = 0xDDDDDDDD;
    if (_undoManager && reinterpret_cast<uintptr_t>(_undoManager) != INVALID_PTR) {
        bool ret = _undoManager->undo();
        CCLOG("撤销操作结果：%d", ret);
    }
    else {
        CCLOGERROR("GameController::handleUndoClick - _undoManager 为空或已释放！");
    }
}