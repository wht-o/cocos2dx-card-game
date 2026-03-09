#pragma once
#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include "CardModel.h"
#include "cocos2d.h" 

/*游戏全局数据模型，存储游戏运行时的所有动态数据（主牌区、底牌堆、当前顶部底牌等） */
class GameModel : public cocos2d::Ref
{
public:
    ~GameModel();

    //创建游戏模型
    static GameModel* create();

    // 添加卡牌到主牌区，card卡牌模型数据
    void addPlayfieldCard(CardModel* card);

    // 获得主牌区卡牌id，cardId卡牌id
    CardModel* getPlayfieldCardById(int cardId);

    // 主牌区通过卡牌id移除主牌区，cardId卡牌id
    void removePlayfieldCardById(int cardId);

    //获得所有主牌区卡牌模型，返回所有主牌区卡牌模型（const只读）
    const cocos2d::Vector<CardModel*>& getPlayfieldCards() const { return _playfieldCards; }

    //  添加卡牌到底牌堆，card卡牌模型数据
    void addStackCard(CardModel* card);

    // 获取底牌堆顶部牌
    CardModel* getStackTopCard();

    // 获得底牌堆指定ID的卡牌，cardId为卡牌唯一ID
    CardModel* getStackCardById(int cardId);

    // 替换底牌堆顶部牌
    void replaceStackTopCard(CardModel* newCard);

    //获得所有底牌堆卡牌模型，返回所有底牌堆卡牌模型（const只读）
    const cocos2d::Vector<CardModel*>& getStackCards() const { return _stackCards; }

    // 序列化
    cocos2d::ValueMap serialize() const;
    // 反序列化
    void deserialize(const cocos2d::ValueMap& data);



private:
    
    cocos2d::Vector<CardModel*> _playfieldCards; // 主牌区卡牌
    cocos2d::Vector<CardModel*> _stackCards;     // 底牌堆卡牌

    bool _init();
};

#endif //GAMEMODEL_H