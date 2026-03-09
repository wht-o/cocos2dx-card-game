#pragma once
#ifndef GAME_MODEL_GENERATOR_H
#define GAME_MODEL_GENERATOR_H

#include "configs/LevelConfig.h"
#include "models/GameModel.h"


// 游戏模型生成器（负责根据关卡配置初始化GameModel）
class GameModelGenerator
{
public:


    // 根据关卡配置生成游戏模型，config 关卡配置，返回游戏模型实例
    static GameModel* generateGameModel(const LevelConfig& config);

private:
    //从卡牌配置结构体，创建卡牌数据模型，config 卡牌配置，cardId 卡牌唯一id，返回卡牌模型
    static CardModel* _createCardModelFromConfig(const CardConfig& config, int cardId);
};

#endif // GAMEMODELGENERATOR_H