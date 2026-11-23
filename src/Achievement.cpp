#include "Achievement.hpp"

using namespace geode::prelude;

namespace ap {

Achievement::Reward Achievement::Reward::fromString(geode::ZStringView string) {
    Achievement::Reward ret;
    if (string.empty())
        return ret;
    
    auto split = geode::utils::string::splitView(string, "_");
    if (split.size() != 2) {
        log::error("Not valid reward: {}", string);
        return ret;    
    }

    if (split[0] == "ship") ret.type = UnlockType::Ship;
    else if (split[0] == "ball") ret.type = UnlockType::Ball;
    else if (split[0] == "bird") ret.type = UnlockType::Bird;
    else if (split[0] == "dart") ret.type = UnlockType::Dart;
    else if (split[0] == "robot") ret.type = UnlockType::Robot;
    else if (split[0] == "spider") ret.type = UnlockType::Spider;
    else if (split[0] == "death") ret.type = UnlockType::Death;
    else if (split[0] == "special") ret.type = UnlockType::Streak;
    else if (split[0] == "swing") ret.type = UnlockType::Swing;
    else if (split[0] == "jetpack") ret.type = UnlockType::Jetpack;
    else if (split[0] == "shipstreak") ret.type = UnlockType::ShipFire;
    else if (split[0] == "color") ret.type = UnlockType::Col1;
    else if (split[0] == "color2") ret.type = UnlockType::Col2;
    else ret.type = UnlockType::Cube;

    auto id = utils::numFromString<int>(split[1]);
    if (!id) {
        log::error("reward id in ({}) is invalid", string);
        return ret;
    }

    ret.id = id.unwrap();
    return ret;
} 

Achievement Achievement::fromCCDict(cocos2d::CCDictionary* dict) {
    Achievement ret;

    ret.identifier = static_cast<CCString*>(dict->objectForKey("identifier"))->getCString();
    ret.title = static_cast<CCString*>(dict->objectForKey("title"))->getCString();
    ret.reward = Achievement::Reward::fromString(static_cast<CCString*>(dict->objectForKey("icon"))->getCString());
    ret.unachievedDescription = static_cast<CCString*>(dict->objectForKey("unachievedDescription"))->getCString();
    ret.achievedDescription = static_cast<CCString*>(dict->objectForKey("achievedDescription"))->getCString();

    return ret;
}

}