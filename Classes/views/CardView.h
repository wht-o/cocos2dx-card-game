#pragma once
#ifndef CARD_VIEW_H
#define CARD_VIEW_H

#include "cocos2d.h"
#include "models/CardModel.h"

/*卡牌视图类，负责单张卡牌的UI展示、触摸事件监听，通过回调通知控制器*/
class CardView : public cocos2d::Layer {
public:
    ~CardView();
    
    //卡牌点击回调
    using CardClickCallback = std::function<void(int)>;

    // 创建卡牌视图，cardModel 卡牌数据模型，返回 卡牌视图实例
    static CardView* create(CardModel* cardModel);

    //设置点击回调，callback 回调函数
    void setCardClickCallback(const CardClickCallback& callback) { _clickCallback = callback; }

    // 移动卡牌到指定位置（带动画），targetPos 目标位置， duration 动画时长
    void moveToPosition(const cocos2d::Vec2& targetPos, float duration = 0.3f);

    //获取卡牌ID，返回卡牌ID
    int getCardId() const { return _cardModel ? _cardModel->getCardId() : -1; }

    //外部触发翻开状态更新（修正：移除多余的 CardView:: 前缀）
    void refreshOpenState();

private:
    CardModel* _cardModel = nullptr;       // 卡牌数据模型（只读）
    CardClickCallback _clickCallback;      // 点击回调
    cocos2d::Sprite* _cardBg = nullptr;    // 卡牌背景
    cocos2d::Sprite* _bigNumber = nullptr; // 大号数字
    cocos2d::Sprite* _smallNumber = nullptr;// 小号数字
    cocos2d::Sprite* _suit = nullptr;      // 花色
    cocos2d::Sprite* _cardBack = nullptr; // 卡牌背面（覆盖状态显示）
    cocos2d::EventListenerTouchOneByOne* _touchListener = nullptr; // 触摸监听（新增，方便控制启用/禁用）

    // 更新卡牌翻开/覆盖显示状态
    void _updateCardOpenState();

    bool _init(CardModel* cardModel);
    void _initUI(); // 初始化卡牌UI
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
};

#endif //CARDVIEW_H