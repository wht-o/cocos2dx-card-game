#include "CardModel.h"

using namespace cocos2d;


CardModel* CardModel::create(int cardId, CardSuitType suit, CardFaceType face,
    const Vec2& pos, bool isOpen) {
    auto model = new CardModel();
    if (model && model->_init(cardId, suit, face, pos, isOpen)) {
        model->autorelease();
        return model;
    }
    CC_SAFE_DELETE(model);
    return nullptr;
}


bool CardModel::_init(int cardId, CardSuitType suit, CardFaceType face,
    const Vec2& pos, bool isOpen) {
    // 校验 cardId 有效性
    CCLOG("CardModel::init - Attempting to initialize cardId: %d", cardId);
    if (cardId < 0) {
        CCLOGERROR("CardModel::init - Invalid cardId: %d (must be >= 0)", cardId);
        return false;
    }
    _cardId = cardId;
    _suit = suit;
    _face = face;
    _position = pos;
    _isOpen = isOpen; 
    return true;
}


ValueMap CardModel::serialize() const {
    ValueMap data;
    data["cardId"] = _cardId;
    data["suit"] = static_cast<int>(_suit);
    data["face"] = static_cast<int>(_face);
    data["posX"] = _position.x;
    data["posY"] = _position.y;
    data["isOpen"] = _isOpen;
    return data;
}


void CardModel::deserialize(const ValueMap& data) {
    _cardId = data.at("cardId").asInt();
    _suit = static_cast<CardSuitType>(data.at("suit").asInt());
    _face = static_cast<CardFaceType>(data.at("face").asInt());
    _position.x = data.at("posX").asFloat();
    _position.y = data.at("posY").asFloat();
    _isOpen = data.at("isOpen").asBool(); // 反序列化翻开状态
}