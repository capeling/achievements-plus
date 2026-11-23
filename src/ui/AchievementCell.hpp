#pragma once

#include <Geode/Geode.hpp>

#include "Achievement.hpp"

namespace ap {

class AchievementCell : public cocos2d::CCNode {
public:
    static AchievementCell* create(const Achievement& achievement, cocos2d::CCSize size);

    void setBGColor(cocos2d::ccColor3B color);

protected:
    bool init(const Achievement& achievement, cocos2d::CCSize size);
};

}