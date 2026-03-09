#include "GameModelGenerator.h"


// GameModelGenerator.cpp
GameModel* GameModelGenerator::generateGameModel(const LevelConfig& config) {
    // 检查 GameModel 创建结果
    auto gameModel = GameModel::create(); // create() 已包含 autorelease()
    if (!gameModel) {
        CCLOG("GameModelGenerator::generateGameModel - 创建GameModel失败！");
        return nullptr;
    }

    int cardId = 0;

    // 生成主牌区卡牌（原有逻辑不变）
    const auto& playfieldConfigs = config.getPlayfieldCards();
    for (const auto& cfg : playfieldConfigs) {
        auto card = _createCardModelFromConfig(cfg, cardId++);
        if (card) {
            gameModel->addPlayfieldCard(card);
        }
        else {
            CCLOG("GameModelGenerator::generateGameModel - 主牌区卡牌创建失败，cardId=%d", cardId - 1);
        }
    }

    // 生成底牌堆卡牌（原有逻辑不变）
    const auto& stackConfigs = config.getStackCards();
    for (const auto& cfg : stackConfigs) {
        auto card = _createCardModelFromConfig(cfg, cardId++);
        if (card) {
            gameModel->addStackCard(card);
        }
        else {
            CCLOG("GameModelGenerator::generateGameModel - 底牌堆卡牌创建失败，cardId=%d", cardId - 1);
        }
    }

    // 调试日志（原有逻辑不变）
    CCLOG("GameModelGenerator::generateGameModel - Playfield card count: %zu, Stack card count: %zu",
        gameModel->getPlayfieldCards().size(),
        gameModel->getStackCards().size());

    return gameModel;
}

CardModel* GameModelGenerator::_createCardModelFromConfig(const CardConfig& config, int cardId) {

    CardSuitType suit = static_cast<CardSuitType>(config.cardSuit);
    CardFaceType face = static_cast<CardFaceType>(config.cardFace);

    bool isOpen = true;

  /*// 根据位置判断是否默认翻开
    if (config.position.y==600)
    {
        isOpen = true;
    }
    else if (config.position.y == 0 )
    {
        isOpen = true;
    }else
    {
        isOpen = false;
    }
   */  


    // 直接传入isOpen参数创建卡牌
    return CardModel::create(cardId, suit, face, config.position, isOpen);
}