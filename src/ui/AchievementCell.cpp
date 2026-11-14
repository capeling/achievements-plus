#include "AchievementCell.hpp"

#include <cue/PlayerIcon.hpp>
#include <cue/Util.hpp>

using namespace geode::prelude;

namespace ap {

AchievementCell* AchievementCell::create(std::shared_ptr<Achievement> achievement, cocos2d::CCSize size) {
    auto ret = new AchievementCell();
    if (ret->init(achievement, size)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool AchievementCell::init(std::shared_ptr<Achievement> achievement, cocos2d::CCSize size) {
    if (!CCNode::init()) {
        return false;
    }

    auto am = AchievementManager::sharedState();
    bool unlocked = am->isAchievementEarned(achievement->identifier.c_str());
    int percent = am->percentForAchievement(achievement->identifier.c_str()); 

    this->setContentSize(size);

    bool achImage = false;

    auto rewardType = achievement->reward.type;
    auto rewardId = achievement->reward.id;
    CCSprite* item;
    if (rewardId == 0) {
        item = CCSprite::createWithSpriteFrameName("GJ_achImage_001.png");
        achImage = true;
    } else {
        item = GJItemIcon::create(rewardType, rewardId, {175, 175, 175}, {255, 255, 255}, false, false, false, {255, 255, 255});
    }

    cue::rescaleToMatch(item, size.height * 0.45f);
    
    if (!achImage && !unlocked) {
        static_cast<GJItemIcon*>(item)->changeToLockedState(1.f / GJItemIcon::scaleForType(achievement->reward.type));
    }

    if (rewardId != 0) {
        auto itemBtn = Button::createWithNode(item, [rewardType, rewardId](auto) {
            ItemInfoPopup::create(rewardId, rewardType)->show();
        });
        this->addChildAtPosition(itemBtn, Anchor::Left, {25.f, 0.f});
    } else {
        this->addChildAtPosition(item, Anchor::Left, {25.f, 0.f});
    }

    // auto testNotify = Button::createWithLabel("Notify", "bigFont.fnt", [achievement, this](auto) {
    //     AchievementManager::sharedState()->notifyAchievementWithID(achievement->identifier.c_str());
    // });
    // this->addChildAtPosition(testNotify, Anchor::Center, {25.f, 0.f});

    auto menu = CCNode::create();
    menu->setLayout(AxisLayout::create(Axis::Column)->setAxisReverse(true)->setCrossAxisLineAlignment(AxisAlignment::Start)->setAutoScale(false)->setGap(2.5f));
    menu->setContentHeight(size.height - 5.f);
    menu->setAnchorPoint({0.f, 0.5f});

    constexpr float LABEL_WIDTH = 180.f;

    auto title = Label::create(achievement->title, "goldFont.fnt");
    title->setAnchorPoint({0.f, 0.5f});
    title->setLimitLabelWidth(LABEL_WIDTH, 0.6f, 0.f);
    menu->addChild(title);
    
    auto desc = Label::create(unlocked ? achievement->achievedDescription : achievement->unachievedDescription, "chatFont.fnt");
    desc->setAnchorPoint({0.f, 0.5f});
    // desc->setLimitLabelWidth(LABEL_WIDTH - 5.f, 0.6f, 0.f);
    desc->setScale(0.6f);
    desc->setMaxWidth(300.f);
    menu->addChild(desc);

    menu->updateLayout();
    this->addChildAtPosition(menu, Anchor::Left, {50.f, 1.f});

    if (percent == 0)
        return true;

    if (!unlocked) {
        auto percentLabel = Label::create(fmt::format("{}%", percent), "goldFont.fnt");
        percentLabel->setScale(0.6f);
        this->addChildAtPosition(percentLabel, Anchor::Right, {-35.f, 0.f});
        return true;
    }

    auto completed = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
    completed->setScale(0.8f);
    this->addChildAtPosition(completed, Anchor::Right, {-35.f, 0.f});

    return true;
}

}
