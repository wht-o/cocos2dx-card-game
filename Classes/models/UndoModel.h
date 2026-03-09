#pragma once
#ifndef UNDOMODEL_H
#define UNDOMODEL_H

#include "CardModel.h"
#include <vector>

enum UndoOpType {
    UOT_STACK_REPLACE,    // 底牌堆替换（手牌区翻牌）
    UOT_PLAYFIELD_MATCH,  // 主牌区匹配替换
};

struct UndoRecord {
    UndoOpType opType;                // 操作类型
    int cardId;                       // 操作的卡牌ID
    CardModel* originalCard;          // 被替换的原始卡牌（底牌堆顶部）
    CardModel* operatedCard;          // 操作的卡牌
    cocos2d::Vec2 originalPosition;   // 操作卡牌的原始位置
};

//回退数据模型保存单次数据信息，存储单次回退所需的所有数据（操作类型、卡牌原始位置、替换的卡牌等）
class UndoModel :public cocos2d::Ref
{
public:
    ~UndoModel();

    //创建回退模型实例。
    static UndoModel* create();

    //添加回退记录,record 回退记录
    void addUndoRecord(const UndoRecord& record);

    //获取最后一条回退记录（并移除）,最后一条记录，无则返回空
    UndoRecord popLastUndoRecord();

    //判断是否有可回退的记录,return true=有，false=无
    bool hasUndoRecord() const { return !_undoRecords.empty(); }

    //清空所有回退记录
    void clearUndoRecords() { _undoRecords.clear(); }

private:
    std::vector<UndoRecord> _undoRecords; // 回退记录列表

    bool _init();
};


#endif // UNDOMODEL_H