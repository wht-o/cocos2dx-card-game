#include "UndoManager.h"
#include "cocos2d.h"

using namespace cocos2d;

UndoManager::UndoManager(UndoModel* undoModel, GameModel* gameModel, GameView* gameView)
    : _undoModel(undoModel)
    , _gameModel(gameModel)
    , _gameView(gameView) {
    CCLOG("UndoManager::构造 - this:%p | undoModel:%p | gameModel:%p | gameView:%p",
        this, undoModel, gameModel, gameView);
}

UndoManager::~UndoManager() {
    CCLOG("UndoManager::析构 - this:%p", this);
}

void UndoManager::addUndoRecord(const UndoRecord& record) {
    if (_undoModel) {
        _undoModel->addUndoRecord(record);
    }
}

bool UndoManager::undo() {
    if (!_undoModel || !_undoModel->hasUndoRecord()) {
        return false;
    }

    auto record = _undoModel->popLastUndoRecord();
    switch (record.opType) {
    case UOT_STACK_REPLACE:
        _handleStackReplaceUndo(record);
        break;
    case UOT_PLAYFIELD_MATCH:
        _handlePlayfieldMatchUndo(record);
        break;
    default:
        break;
    }
    return true;
}

void UndoManager::_handleStackReplaceUndo(const UndoRecord& record) {
    
    // 恢复底牌堆顶部牌
    if (_gameModel && record.originalCard) {
        _gameModel->replaceStackTopCard(record.originalCard);
    }

    
    // 卡牌移回原始位置
    if (_gameView && record.operatedCard) {
        _gameView->moveCardToPosition(record.operatedCard->getCardId(),  record.originalPosition);
    }
}

void UndoManager::_handlePlayfieldMatchUndo(const UndoRecord& record) {

   
    // 恢复底牌堆顶部牌
    if (_gameModel && record.originalCard) {
        _gameModel->replaceStackTopCard(record.originalCard);
    }
    // 恢复主牌区卡牌
    if (_gameModel && record.operatedCard) {
        _gameModel->addPlayfieldCard(record.operatedCard);
    }
   
    // 卡牌移回原始位置
    if (_gameView && record.operatedCard) {
        _gameView->moveCardToPosition(record.operatedCard->getCardId(),  record.originalPosition);
    }
}