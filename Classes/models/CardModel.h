#pragma once
#ifndef CARDMODEL_H
#define CARDMODEL_H

#include "cocos2d.h"

// 花色类型
enum CardSuitType {
	CST_NONE = -1,
	CST_CLUBS,    // 梅花
	CST_DIAMONDS, // 方块
	CST_HEARTS,   // 红桃
	CST_SPADES,   // 黑桃
	CST_NUM_CARD_SUIT_TYPES
};

// 正面类型
enum CardFaceType {
	CFT_NONE = -1,
	CFT_ACE,    // A
	CFT_TWO,    // 2
	CFT_THREE,  // 3
	CFT_FOUR,   // 4
	CFT_FIVE,   // 5
	CFT_SIX,    // 6
	CFT_SEVEN,  // 7
	CFT_EIGHT,  // 8
	CFT_NINE,   // 9
	CFT_TEN,    // 10
	CFT_JACK,   // J
	CFT_QUEEN,  // Q
	CFT_KING,   // K
	CFT_NUM_CARD_FACE_TYPES
};

/*卡牌数据模型，储存单张卡牌核心数据（花色、牌面、位置、ID等）。保存数据，无业务逻辑，支持序列化和反序列化*/
class CardModel : public cocos2d::Ref
{
public:
	

	/*创造单张卡牌实例。cardId 卡牌唯一ID,suit：花色，face：面值，position:卡牌初始位置，isOpen：卡牌是否翻开（为true—翻开，默认false-未翻开）。*/
	static CardModel* create(int cardId, CardSuitType suit, CardFaceType face,
		const cocos2d::Vec2& pos, bool isOpen = false);

	//获取卡牌id
	int getCardId() const { return _cardId; }
	// 设置卡牌id
	void setCardId(int id) { _cardId = id; }

	//获取卡牌花色
	CardSuitType getSuit() const { return _suit; }
	// 设置卡牌花色
	void setSuit(CardSuitType suit) { _suit = suit; }

	//获取卡牌面值
	CardFaceType getFace()const { return _face; };
	//设置卡牌面值
	void setFace(CardFaceType face) { _face = face; }

	//获取卡牌当前位置
	cocos2d::Vec2 getPosition()const { return _position; };

	//设置卡牌当前位置
	void setPosition(const cocos2d::Vec2& pos) { _position = pos; };

	//获取是否翻开（true-翻开）
	bool isCardOpen() const { return _isOpen; }
	//设置是否翻开（true-是）
	void setCardOpen(bool isOpen) { _isOpen = isOpen; }

	// 序列化（存档用）
	cocos2d::ValueMap serialize() const;
	// 反序列化（读档用）
	void deserialize(const cocos2d::ValueMap& data);

private:
    int _cardId = -1;// 卡牌唯一ID
    CardSuitType _suit= CardSuitType::CST_NONE;// 花色
    CardFaceType _face= CardFaceType::CFT_NONE;// 牌面
    cocos2d::Vec2 _position;// 位置
    bool _isOpen = false;// 是否翻开（默认覆盖）

	/*初始化卡牌模型。cardId 卡牌唯一ID,suit：花色，face：面值，position:卡牌初始位置，isOpen：卡牌是否翻开（为true—翻开，默认false-未翻开），返回初始化是否成功。*/
	bool _init(int cardId, CardSuitType suit, CardFaceType face,const cocos2d::Vec2& pos, bool isOpen);
};

#endif // CARDMODEL_H