#pragma once
#ifndef CARD_MATCH_SERVICE_H
#define CARD_MATCH_SERVICE_H

#include "models/CardModel.h"

/*卡牌匹配服务类,提供无状态的卡牌匹配规则判断（点数差1）*/
class CardMatchService {
public:
    /* 判断两张卡牌是否满足匹配规则（点数差1）,cardA 卡牌A（底牌堆顶部）,cardB 卡牌B（主牌区/手牌区） ，返回true=匹配，false=不匹配*/
    static bool isCardMatch(CardModel* cardA, CardModel* cardB);
};

#endif // CARD_MATCH_SERVICE_H