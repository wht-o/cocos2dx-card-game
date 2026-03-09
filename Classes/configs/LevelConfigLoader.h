#pragma once
#ifndef LEVELCONFIGLOADER_H
#define LEVELCONFIGLOADER_H

#include "json/document.h"
#include "configs/LevelConfig.h"
#include "cocos2d.h"

// 关卡配置加载器，从JSON配置加载关卡数据到LevelConfig对象
class LevelConfigLoader
{
public:
    LevelConfigLoader();
    ~LevelConfigLoader();

    // 加载指定关卡配置，levelId关卡id，返回加载后的LevelConfig对象
    static LevelConfig loadLevelConfig(int levelId);

private:
    // 解析JSON节点为CardConfig，cardNode JSON节点卡牌配置
    static CardConfig _parseCardConfig(const rapidjson::Value& cardNode);
};

#endif // LEVELCONFIGLOADER_H