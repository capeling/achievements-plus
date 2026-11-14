#pragma once

#include <Geode/Geode.hpp>

#include "Achievement.hpp"

namespace ap {

class AchievementCell : public cocos2d::CCNode {
public:
    static AchievementCell* create(std::shared_ptr<Achievement> achievement, cocos2d::CCSize size);

    void setBGColor(cocos2d::ccColor3B color);

protected:
    bool init(std::shared_ptr<Achievement> achievement, cocos2d::CCSize size);
};

}