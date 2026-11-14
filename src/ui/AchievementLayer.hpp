#pragma once

#include <Geode/Geode.hpp>
#include <cue/ListNode.hpp>
#include <Geode/ui/Popup.hpp>

#include "ModManager.hpp"

namespace ap {

enum class FilterType : uint8_t {
    None = 0,
    Progress,
    Completed,
    Uncompleted,
    Last
};

class AchievementLayer : public geode::Popup {
public:
    static constexpr cocos2d::CCSize POPUP_SIZE = {435.f, 275.f};
    static constexpr cocos2d::CCSize LIST_SIZE = POPUP_SIZE - cocos2d::CCSize{85.f, 85.f};

    static AchievementLayer* create();

protected:
    bool init();

    void loadCategory(AchievementCategory category);
    void createCategoryButtons();
    void createFilterButtons();

    cue::ListNode* m_achievementList = nullptr;
    geode::Label* m_countLabel = nullptr;
    cocos2d::CCMenu* m_categoryMenu = nullptr;
    cocos2d::CCMenu* m_filterMenu = nullptr;
    AchievementCategory m_currentCategory = AchievementCategory::MainLevels;
    FilterType m_filterType = FilterType::None;
};

}