#include "CardMatchService.h"

bool CardMatchService::isCardMatch(CardModel* cardA, CardModel* cardB) {
    if (!cardA || !cardB) {
        return false;
    }
    int faceA = static_cast<int>(cardA->getFace());
    int faceB = static_cast<int>(cardB->getFace());
    return abs(faceA - faceB) == 1;
}