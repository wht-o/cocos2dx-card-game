#pragma once
#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "cocos2d.h"
#include "views/CardView.h"
#include "models/GameModel.h"

/* 游戏总视图，管理所有子视图（桌面牌区、手牌区、回退按钮）布局管理，管理所有UI组件（主牌区、底牌堆、回退按钮），转发触摸事件 */
class GameView : public cocos2d::Layer {
public:

    
    // 回退按钮点击回调
    using UndoClickCallback = std::function<void()>;
    // 卡牌点击回调
    using CardClickCallback = std::function<void(int)>;

    // 创造游戏总视图,gameModel游戏模型
    static GameView* create(GameModel* gameModel);

    // 设置卡牌点击回调
    void setCardClickCallback(const CardClickCallback& callback) { _cardClickCallback = callback; }
    // 设置回退按钮点击回调
    void setUndoClickCallback(const UndoClickCallback& callback) { _undoClickCallback = callback; }

    // 移动指定ID的卡牌到目标位置
    void moveCardToPosition(int cardId, const cocos2d::Vec2& targetPos);

    // 更新底牌堆顶部卡牌视图
    void updateStackTopCard(CardModel* newCardModel);

    // 刷新指定ID卡牌的翻开状态
    void refreshCardOpenState(int cardId);

    // 移除指定ID的卡牌视图（从场景和映射表中删除）
    void removeCardView(int cardId);



private:
    GameModel* _gameModel = nullptr; // 游戏数据模型（只读）
    CardClickCallback _cardClickCallback; // 卡牌点击回调
    UndoClickCallback _undoClickCallback; // 回退按钮回调
    std::map<int, CardView*> _cardViews;  // 卡牌视图映射（ID -> 视图）
    CardView* _stackTopCardView = nullptr; // 底牌堆顶部卡牌视图
    cocos2d::MenuItemImage* _undoBtn = nullptr; // 回退按钮

    bool _init(GameModel* gameModel);
    // 初始化主牌区卡牌
    void _initPlayfieldCards();
    // 初始化底牌堆卡牌
    void _initStackCards();
    // 初始化回退按钮
    void _initUndoButton();
    // 回退按钮点击事件
    void _onUndoBtnClicked();

    // 清理无效的卡牌视图指针
    void _cleanInvalidCardViews();

    // 检查当前GameView是否为有效节点
    bool _isGameViewValid() const {
        return this != nullptr           // 指针非空
            && static_cast<const cocos2d::Node*>(this)->isRunning(); // 节点在运行
    }
};

#endif // GAMEVIEW_H