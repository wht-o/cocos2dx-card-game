#include "CardResUtils.h"

using namespace cocos2d;

std::string CardResUtils::getSuitResPath(CardSuitType suit) {
    switch (suit) {
    case CST_CLUBS: return "res/suits/club.png";
    case CST_DIAMONDS: return "res/suits/diamond.png";
    case CST_HEARTS: return "res/suits/heart.png";
    case CST_SPADES: return "res/suits/spade.png";
    default: return "";
    }
}

bool CardResUtils::isSuitRed(CardSuitType suit) {
    return suit == CST_DIAMONDS || suit == CST_HEARTS;
}

std::string CardResUtils::faceTypeToString(CardFaceType face) {
    switch (face) {
    case CFT_ACE: return "A";
    case CFT_TWO: return "2";
    case CFT_THREE: return "3";
    case CFT_FOUR: return "4";
    case CFT_FIVE: return "5";
    case CFT_SIX: return "6";
    case CFT_SEVEN: return "7";
    case CFT_EIGHT: return "8";
    case CFT_NINE: return "9";
    case CFT_TEN: return "10";
    case CFT_JACK: return "J";
    case CFT_QUEEN: return "Q";
    case CFT_KING: return "K";
    default: return "";
    }
}

std::string CardResUtils::getBigNumberResPath(CardFaceType face, bool isRed) {
    std::string color = isRed ? "Red" : "black";
    std::string faceStr = faceTypeToString(face);
    return StringUtils::format("res/number/big_%s_%s.png", color.c_str(), faceStr.c_str());
}

std::string CardResUtils::getSmallNumberResPath(CardFaceType face, bool isRed) {
    std::string color = isRed ? "Red" : "black";
    std::string faceStr = faceTypeToString(face);
    return StringUtils::format("res/number/small_%s_%s.png", color.c_str(), faceStr.c_str());
}