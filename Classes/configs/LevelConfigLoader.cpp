#include "LevelConfigLoader.h"
// 引入Cocos2d-x内置的rapidjson头文件
#include "json/document.h"

using namespace cocos2d;


// 构造/析构函数实现
LevelConfigLoader::LevelConfigLoader() {}
LevelConfigLoader::~LevelConfigLoader() {}

LevelConfig LevelConfigLoader::loadLevelConfig(int levelId) {
    LevelConfig config;
    // 拼接关卡配置文件路径
    std::string configPath = StringUtils::format("Levels/level_%d.json", levelId);
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(configPath);

    // 调试日志：打印完整路径和文件存在性
    CCLOG("=== LevelConfigLoader Debug ===");
    CCLOG("Config relative path: %s", configPath.c_str());
    CCLOG("Config full path: %s", fullPath.c_str());
    CCLOG("File exists: %d", FileUtils::getInstance()->isFileExist(fullPath));

    // 检查文件是否存在
    if (!FileUtils::getInstance()->isFileExist(fullPath)) {
        CCLOGERROR("Level config file not found: %s", fullPath.c_str());
        return config;
    }

    //手动读取JSON文件内容
    std::string jsonContent = FileUtils::getInstance()->getStringFromFile(fullPath);
    if (jsonContent.empty()) {
        CCLOGERROR("Read JSON file failed, content is empty! Path: %s", fullPath.c_str());
        return config;
    }
    CCLOG("JSON content length: %zu", jsonContent.length());

    // 使用rapidjson解析JSON字符串
    rapidjson::Document doc;
    doc.Parse(jsonContent.c_str());

    // 检查解析错误
    if (doc.HasParseError()) {
        CCLOGERROR("JSON parse error! Error code: %d, Offset: %zu",
            doc.GetParseError(), doc.GetErrorOffset());
        return config;
    }
    // 校验根节点是否为JSON对象
    if (!doc.IsObject()) {
        CCLOGERROR("JSON root is not an object!");
        return config;
    }
    //

    // 解析主牌区
    if (doc.HasMember("Playfield") && doc["Playfield"].IsArray()) {
        const rapidjson::Value& playfieldArr = doc["Playfield"];
        CCLOG("Playfield array size: %zu", playfieldArr.Size());
        std::vector<CardConfig> playfieldCards;
        // 明确rapidjson::SizeType
        for (rapidjson::SizeType i = 0; i < playfieldArr.Size(); ++i) {
            const rapidjson::Value& cardVal = playfieldArr[i];
            if (!cardVal.IsObject()) {
                CCLOGERROR("Playfield[%zu] is not a map! Type: %d", i, cardVal.GetType());
                continue;
            }
            playfieldCards.push_back(_parseCardConfig(cardVal));
        }
        config.setPlayfieldCards(playfieldCards);
        CCLOG("Parsed Playfield cards: %zu", playfieldCards.size());
    }
    else {
        CCLOGWARN("Playfield key not found or not an array!");
    }

    // 解析底牌堆
    if (doc.HasMember("Stack") && doc["Stack"].IsArray()) {
        const rapidjson::Value& stackArr = doc["Stack"];
        CCLOG("Stack array size: %zu", stackArr.Size());
        std::vector<CardConfig> stackCards;
        for (rapidjson::SizeType i = 0; i < stackArr.Size(); ++i) {
            const rapidjson::Value& cardVal = stackArr[i];
            if (!cardVal.IsObject()) {
                CCLOGERROR("Stack[%zu] is not a map! Type: %d", i, cardVal.GetType());
                continue;
            }
            stackCards.push_back(_parseCardConfig(cardVal));
        }
        config.setStackCards(stackCards);
        CCLOG("Parsed Stack cards: %zu", stackCards.size());
    }
    else {
        CCLOGWARN("Stack key not found or not an array!");
    }

    return config;
}

//  适配rapidjson的卡牌解析函数（明确命名空间） 
CardConfig LevelConfigLoader::_parseCardConfig(const rapidjson::Value& cardNode) {
    CardConfig cardConfig;

    // 解析卡牌花色/点数
    if (cardNode.HasMember("CardFace") && cardNode["CardFace"].IsInt()) {
        cardConfig.cardFace = cardNode["CardFace"].GetInt();
        CCLOG("CardFace: %d", cardConfig.cardFace);
    }
    else {
        CCLOGWARN("Card missing CardFace or not int!");
        cardConfig.cardFace = 0; // 默认值
    }

    if (cardNode.HasMember("CardSuit") && cardNode["CardSuit"].IsInt()) {
        cardConfig.cardSuit = cardNode["CardSuit"].GetInt();
        CCLOG("CardSuit: %d", cardConfig.cardSuit);
    }
    else {
        CCLOGWARN("Card missing CardSuit or not int!");
        cardConfig.cardSuit = 0; // 默认值
    }

    // 解析卡牌位置
    if (cardNode.HasMember("Position") && cardNode["Position"].IsObject()) {
        const rapidjson::Value& posNode = cardNode["Position"];
        float x = posNode.HasMember("x") && posNode["x"].IsNumber() ? posNode["x"].GetFloat() : 0.0f;
        float y = posNode.HasMember("y") && posNode["y"].IsNumber() ? posNode["y"].GetFloat() : 0.0f;
        cardConfig.position = Vec2(x, y);
        CCLOG("Position: (%.1f, %.1f)", x, y);
    }
    else {
        CCLOGWARN("Card missing Position or not object!");
        cardConfig.position = Vec2::ZERO; // 无Position时默认原点
    }

    return cardConfig;
}