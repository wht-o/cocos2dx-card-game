#pragma once
#ifndef LEVEL_CONFIG_H
#define LEVEL_CONFIG_H

#include "cocos2d.h"
#include <vector>

// 卡牌配置结构体
struct CardConfig {
    int cardFace;      // 牌面数字（对应CardFaceType）
    int cardSuit;      // 花色（对应CardSuitType）
    cocos2d::Vec2 position; // 初始位置
};

// 关卡配置类
class LevelConfig {
public:

    LevelConfig() = default;

    //获取主牌区配置,返回主牌区卡牌配置列表 
    const std::vector<CardConfig>& getPlayfieldCards() const { return _playfieldCards; }

    //获取底牌堆配置，返回底牌堆卡牌配置列表
    const std::vector<CardConfig>& getStackCards() const { return _stackCards; }

    // 设置配置桌面牌区数据
    void setPlayfieldCards(const std::vector<CardConfig>& cards) { _playfieldCards = cards; }

    // 设置配置手牌区数据
    void setStackCards(const std::vector<CardConfig>& cards) { _stackCards = cards; }

private:
    std::vector<CardConfig> _playfieldCards; // 主牌区卡牌配置
    std::vector<CardConfig> _stackCards;     // 底牌堆卡牌配置
};

#endif // LEVEL_CONFIG_H