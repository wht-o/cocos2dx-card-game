#pragma once

#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "models/UndoModel.h"
#include "models/GameModel.h"
#include "views/GameView.h"


/*回退管理器，负责处理回退逻辑；处理回退逻辑，协调UndoModel、GameModel和GameView的回退操作，作为Controller成员，非单例*/
class UndoManager
{
public:
	/*构造函数，undoModel回退数据模型，gameModel游戏全局数据模型，游戏总视图*/
    UndoManager(UndoModel* undoModel, GameModel* gameModel, GameView* gameView);
	~UndoManager();

	//执行1次回退操作，返回true回退成功，false回退失败
    bool undo();

    //添加回退记录，record回退记录
    void addUndoRecord(const UndoRecord& record);

   

private:

    UndoModel* _undoModel = nullptr; // 回退数据模型
    GameModel* _gameModel = nullptr; // 游戏数据模型
    GameView* _gameView = nullptr;   // 游戏视图

    //处理底牌堆替换的回退，record 回退记录
    void _handleStackReplaceUndo(const UndoRecord& record);

    //处理主牌区匹配的回退，record 回退记录
    void _handlePlayfieldMatchUndo(const UndoRecord& record);
};





#endif //UNDOMANAGER_H