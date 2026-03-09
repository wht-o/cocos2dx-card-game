#include "UndoModel.h"

UndoModel::~UndoModel() {
    // 释放所有未处理的UndoRecord中的卡牌
    for (auto& record : _undoRecords) {
        if (record.originalCard) record.originalCard->release();
        if (record.operatedCard) record.operatedCard->release();
    }
    _undoRecords.clear();
}

UndoModel* UndoModel::create() {
    auto model = new UndoModel();
    if (model && model->_init()) {
        model->autorelease();
        return model;
    }
    CC_SAFE_DELETE(model);
    return nullptr;
}

bool UndoModel::_init() {
    return true;
}

void UndoModel::addUndoRecord(const UndoRecord& record) {
    UndoRecord copyRecord = record;
    // 持有卡牌引用，防止被提前释放
    if (copyRecord.originalCard) copyRecord.originalCard->retain();
    if (copyRecord.operatedCard) copyRecord.operatedCard->retain();
    _undoRecords.push_back(copyRecord);
}
UndoRecord UndoModel::popLastUndoRecord() {
    if (_undoRecords.empty()) {
        return { UOT_STACK_REPLACE, -1, nullptr, nullptr, cocos2d::Vec2::ZERO };
    }
    auto record = _undoRecords.back();
    _undoRecords.pop_back();
    return record;
}