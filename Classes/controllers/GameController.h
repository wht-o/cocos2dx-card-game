#pragma once
#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "cocos2d.h"
#include "models/GameModel.h"
#include "models/CardModel.h"
#include "models/UndoModel.h"
#include "views/GameView.h"
#include "managers/UndoManager.h"
#include "services/CardMatchService.h"
#include "services/GameModelGenerator.h"
#include "configs/LevelConfigLoader.h"
#include "configs/LevelConfig.h" 

// 游戏控制器,协调模型和视图，处理核心游戏逻辑（卡牌点击、匹配、回退等）
class GameController : public cocos2d::Ref {
public:
    ~GameController();
  
    // 创建游戏实例，levelId关卡id
    static GameController* create(int levelId);

    // 启动游戏
    void startGame();

    // 处理卡牌点击事件,cardId 被点击的卡牌ID,返回 true=处理成功，false=处理失败
    bool handleCardClick(int cardId);

    // 处理回退按钮点击事件
    void handleUndoClick();

    //翻开指定卡牌下方的覆盖卡牌,cardId 被点击的卡牌ID
    void openUnderCard(int cardId);
private:
    int _levelId = 0;                          // 当前关卡ID
    GameModel* _gameModel = nullptr;           // 游戏数据模型
    UndoModel* _undoModel = nullptr;           // 回退数据模型
    GameView* _gameView = nullptr;             // 游戏视图
    UndoManager* _undoManager = nullptr;       // 回退管理器

    bool _init(int levelId);

    // 处理手牌区翻牌替换逻辑， cardId 手牌区卡牌ID
    void _handleStackReplace(int cardId);

    //处理主牌区卡牌匹配逻辑， cardId 主牌区卡牌ID
    void _handlePlayfieldMatch(int cardId);

};

#endif // GAME_CONTROLLER_H