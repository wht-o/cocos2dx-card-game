#include "GameModel.h"
#include "cocos2d.h"

using namespace cocos2d;

GameModel::~GameModel() {
    CCLOG("GameModel::~GameModel - Destructor | Number of cards in game area: %zu | Number of cards in stack area: %zu",
        _playfieldCards.size(), _stackCards.size());

    _playfieldCards.clear();
    _stackCards.clear();

}

GameModel* GameModel::create() {
    auto model = new GameModel();
    if (model && model->_init()) {
        model->autorelease();
        return model;
    }
    CC_SAFE_DELETE(model);
    return nullptr;
}

bool GameModel::_init() {
    return true;
}

void GameModel::addPlayfieldCard(CardModel* card) {
    if (card) {
        _playfieldCards.pushBack(card);
    }
}

CardModel* GameModel::getPlayfieldCardById(int cardId) {
   

    // 1. 非法cardId检查
    if (cardId < 0) {
        CCLOGERROR("GameModel::getPlayfieldCardById - Invalid cardId: %d (must be >= 0)", cardId);
        return nullptr;
    }
   

    // 2. 修复：检查GameModel自身+容器有效性（适配cocos2d::Vector）
    if (this == nullptr) { // 先检查自身是否为空
        CCLOGERROR("GameModel::getPlayfieldCardById - GameModel instance is null!");
        return nullptr;
    }
   
    // 注：cocos2d::Vector被销毁后，size()访问会触发异常，因此先通过Ref的引用计数判断
    if (this->getReferenceCount() <= 0) {
        CCLOGERROR("GameModel::getPlayfieldCardById - GameModel is already destroyed (ref count <= 0)");
        return nullptr;
    }

    // 3. 游戏区无卡牌检查
    if (_playfieldCards.empty()) {
        CCLOG("GameModel::getPlayfieldCardById - No cards in game area");
        return nullptr;
    }
   

    // 4. 遍历匹配卡牌（增加card空指针检查）
    for (auto card : _playfieldCards) {
        if (!card) { // 跳过空指针卡牌
            CCLOGWARN("GameModel::getPlayfieldCardById - Skip null card in playfield (cardId: %d)", cardId);
            continue;
        }
        if (card->getCardId() == cardId) {
            return card;
        }
    }

    CCLOGWARN("GameModel::getPlayfieldCardById - Card with cardId: %d not found", cardId);
    return nullptr;
}

CardModel* GameModel::getStackCardById(int cardId) {
    // 1. 非法cardId检查
    if (cardId < 0) {
        CCLOGERROR("GameModel::getStackCardById - Invalid cardId: %d (must be >= 0)", cardId);
        return nullptr;
    }

    // 2. 检查GameModel自身有效性（防止空指针/已销毁）
    if (this == nullptr) {
        CCLOGERROR("GameModel::getStackCardById - GameModel instance is null!");
        return nullptr;
    }
    if (this->getReferenceCount() <= 0) {
        CCLOGERROR("GameModel::getStackCardById - GameModel is already destroyed (ref count <= 0)");
        return nullptr;
    }

    // 3. 底牌堆无卡牌检查
    if (_stackCards.empty()) {
        CCLOG("GameModel::getStackCardById - No cards in stack area");
        return nullptr;
    }

    // 4. 遍历底牌堆匹配卡牌（跳过空指针卡牌）
    for (auto card : _stackCards) {
        if (!card) {
            CCLOGWARN("GameModel::getStackCardById - Skip null card in stack (cardId: %d)", cardId);
            continue;
        }
        if (card->getCardId() == cardId) {
            return card;
        }
    }

    CCLOGWARN("GameModel::getStackCardById - Card with cardId: %d not found in stack", cardId);
    return nullptr;
}

void GameModel::removePlayfieldCardById(int cardId) {
    // 遍历查找并移除
    for (auto card : _playfieldCards) {
        if (card && card->getCardId() == cardId) {
          
            _playfieldCards.eraseObject(card);
            break;
        }
    }
}

void GameModel::addStackCard(CardModel* card) {
    if (card) {
        _stackCards.pushBack(card);
    }
}

CardModel* GameModel::getStackTopCard() {
    if (_stackCards.empty()) {
        CCLOGERROR("GameModel::getStackTopCard - Stack top card is null!");
        return nullptr;
    }
    return _stackCards.back();
}

void GameModel::replaceStackTopCard(CardModel* newCard) {
    if (_stackCards.empty() || !newCard) return;

    // 释放旧栈顶卡牌（
    auto oldCard = _stackCards.back();
    _stackCards.eraseObject(oldCard);


    // 添加新栈顶卡牌（
    _stackCards.pushBack(newCard);
}

ValueMap GameModel::serialize() const {
    ValueMap data;
    ValueVector playfieldVec;
   
    for (auto card : _playfieldCards) {
        playfieldVec.push_back(Value(card->serialize()));
    }
    data["playfield"] = playfieldVec;

    ValueVector stackVec;
    for (auto card : _stackCards) {
        stackVec.push_back(Value(card->serialize()));
    }
    data["stack"] = stackVec;
    return data;
}

void GameModel::deserialize(const ValueMap& data) {
    // 清空原有数据（自动release旧卡牌）
    _playfieldCards.clear();
    _stackCards.clear();

    // 解析主牌区
    if (data.find("playfield") != data.end()) {
        ValueVector playfieldVec = data.at("playfield").asValueVector();
        for (const auto& val : playfieldVec) {
            auto card = CardModel::create(-1, CST_NONE, CFT_NONE, Vec2::ZERO);
            card->deserialize(val.asValueMap());
            _playfieldCards.pushBack(card); // pushBack自动retain
        }
    }

    // 解析底牌堆
    if (data.find("stack") != data.end()) {
        ValueVector stackVec = data.at("stack").asValueVector();
        for (const auto& val : stackVec) {
            auto card = CardModel::create(-1, CST_NONE, CFT_NONE, Vec2::ZERO);
            card->deserialize(val.asValueMap());
            _stackCards.pushBack(card); // pushBack自动retain
        }
    }
}