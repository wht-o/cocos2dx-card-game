#pragma once
#ifndef CARDUTILS_H
#define CARDUTILS_H

#include "models/CardModel.h"
#include "cocos2d.h"

/*卡牌工具类，提供卡牌图片资源的路径拼接、颜色判断等通用功能*/
class CardResUtils
{
public:

    //获取花色图片路径,suit 花色类型,返回图片路径
    static std::string getSuitResPath(CardSuitType suit);

    //获取大号数字图片路径， face 牌面类型，isRed 是否红色（红桃/方块为红，梅花/黑桃为黑），返回图片路径
    static std::string getBigNumberResPath(CardFaceType face, bool isRed);

    ////获取小号数字图片路径， face 牌面类型，isRed 是否红色（红桃/方块为红，梅花/黑桃为黑），返回图片路径
    static std::string getSmallNumberResPath(CardFaceType face, bool isRed);

    //判断花色是否为红色，suit 花色类型,返回true=红色，false=黑色
    static bool isSuitRed(CardSuitType suit);

    //将牌面类型转为字符串（用于资源名），face 牌面类型，返回字符串（如A、2、J等）
    static std::string faceTypeToString(CardFaceType face);


private:

};


#endif // CARDUTILS_H