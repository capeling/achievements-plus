#include "AchievementLayer.hpp"

#include "AchievementCell.hpp"

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <Geode/external/fts/fts_fuzzy_match.h>

using namespace geode::prelude;

namespace ap {

static bool weightedFuzzyMatch(ZStringView str, ZStringView kw, double weight, double& out) {
    int score;
    if (fts::fuzzy_match(kw.c_str(), str.c_str(), score)) {
        out = std::max(out, score * weight);
        return true;
    }
    return false;
}

static std::string getFilterSpriteName(FilterType type) {
    switch (type) {
        using enum FilterType;
        case Completed:
            return "completedFilter.png"_spr;
        case Uncompleted:
            return "uncompletedFilter.png"_spr;
        case Progress:
            return "progressFilter.png"_spr;
        default:
            return "noFilter.png"_spr;
    }
    return "noFilter.png";
}

static std::string filterTypeToNodeID(FilterType type) {
    switch (type) {
        using enum FilterType;
        case Completed:
            return "completed-filter";
        case Uncompleted:
            return "uncompleted-filter";
        case Progress:
            return "progress-filter";
        default:
            return "no-filter";
    }
    return "unknown-filter";
}

AchievementLayer* AchievementLayer::create() {
    auto ret = new AchievementLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool AchievementLayer::init() {
    if (!Popup::init(POPUP_SIZE)) {
        return false;
    }

    m_achievementList = cue::ListNode::create(LIST_SIZE);
    m_achievementList->setCellColors<ccColor3B, ccColor3B>({161, 88, 44}, {194, 114, 62});
    m_achievementList->setID("achievement-list");
    m_mainLayer->addChildAtPosition(m_achievementList, Anchor::Center, {0, -25.f});

    m_searchMenu = CCNode::create();
    m_searchMenu->ignoreAnchorPointForPosition(false);
    m_searchMenu->setID("search-node");
    m_searchMenu->setContentSize({LIST_SIZE.width, 30});

    auto searchBG = CCLayerColor::create({194, 114, 62, 255});
    searchBG->setContentSize(m_searchMenu->getContentSize());
    searchBG->setID("search-background");
    searchBG->ignoreAnchorPointForPosition(false);
    m_searchMenu->addChildAtPosition(searchBG, Anchor::Center);

    auto searchInput = TextInput::create(LIST_SIZE.width - 35.f, "Search Achievements");
    searchInput->setScale(0.75f);
    searchInput->setID("search-input");
    searchInput->setAnchorPoint({0.f, 0.5f});
    searchInput->setAlignment(Label::Alignment::Left);
    searchInput->setCommonFilter(CommonFilter::Any);
    searchInput->setCallback([this](const std::string& str) {
        m_searchQuery = utils::string::trim(str);
        loadCategory(m_currentCategory);
    });

    m_searchMenu->addChildAtPosition(searchInput, Anchor::Left, {7.5f, 0.f});
    m_searchMenu->setPosition(0, 160);

    m_achievementList->getScrollLayer()->m_scrollLimitTop += m_searchMenu->getScaledContentHeight();
    m_achievementList->getScrollLayer()->scrollLayer(-m_searchMenu->getScaledContentHeight());
    m_achievementList->addChild(m_searchMenu);

    this->loadCategory(AchievementCategory::MainLevels);
    this->createCategoryButtons();
    this->createFilterButtons();

    this->setID("achievement-layer"_spr);
    return true;
}

void AchievementLayer::loadCategory(AchievementCategory category) {
    m_currentCategory = category;
    m_achievementList->clear();

    cue::resetNode(m_countLabel);

    auto& mm = ModManager::get();
    auto am = AchievementManager::sharedState();

    auto achsSpan = mm.getAchievementsInCategory(category);
    auto achs = std::vector<ModManager::AchievementPtr>(achsSpan.begin(), achsSpan.end());

    switch (m_filterType) {
        using enum FilterType;
        case None:
            break;
        case Progress:
            std::sort(achs.begin(), achs.end(), [](const ModManager::AchievementPtr& a, const ModManager::AchievementPtr& b) {
                auto am = AchievementManager::sharedState();
                auto ap = am->percentForAchievement(a->identifier.c_str());
                auto bp = am->percentForAchievement(b->identifier.c_str());

                return ap > bp;
            });
            break;
        case Uncompleted:
            for (auto it = achs.begin(); it != achs.end(); ) {
                auto obj = *it;
                if (am->isAchievementEarned(obj->identifier.c_str())) {
                    it = achs.erase(it);
                    continue;
                }
                ++it;
            }
            break;
        case Completed:
            for (auto it = achs.begin(); it != achs.end(); ) {
                auto obj = *it;
                if (!am->isAchievementEarned(obj->identifier.c_str())) {
                    it = achs.erase(it);
                    continue;
                }
                ++it;
            }
            break;
        default:
            break;
    }

    int filteredCount = 0;
    for (auto& ach : achs) {
        if (m_searchQuery.empty() || matchSearch(ach, m_searchQuery)) {
            m_achievementList->addCell(AchievementCell::create(ach, {LIST_SIZE.width, 45.f}));
            filteredCount++;
        }
    }

    m_achievementList->getScrollLayer()->scrollLayer(-m_searchMenu->getScaledContentHeight());

    m_countLabel = Label::create(fmt::format("{}/{} Achievements", filteredCount, am->m_allAchievementsSorted->count()), "goldFont.fnt");
    m_countLabel->setScale(0.4f);
    auto winSize = CCDirector::get()->getWinSize();
    m_countLabel->setID("count-label");
    this->addChild(m_countLabel);
    m_countLabel->setPosition(winSize.width - m_countLabel->getScaledContentWidth() / 2.f - 5.f, winSize.height - m_countLabel->getScaledContentHeight() + 2.5f);
    this->setTitle(getAchievementCategoryName(category));
}

void AchievementLayer::createCategoryButtons() {
    cue::resetNode(m_categoryMenu);
    m_categoryMenu = CCMenu::create();
    for (int i = (int)AchievementCategory::None + 1; i < (int)AchievementCategory::Last; i++) {
        std::string categoryName = getAchievementCategoryName((AchievementCategory)i);

        std::string buttonText = utils::string::replace(categoryName, " ", "\n");
        std::string buttonID = utils::string::replace(categoryName, " ", "-");
        buttonID = utils::string::toLower(buttonID) + "-button";

        bool isSelected = (AchievementCategory)i == m_currentCategory;

        auto btnSprite = ButtonSprite::create(buttonText.c_str(), 58.f, 0.f, 0.266f, false, "bigFont.fnt", isSelected ? "GJ_button_02.png" : "GJ_button_05.png", 25.f);
        auto button = Button::createWithNode(btnSprite, [this, i](auto) {
            this->loadCategory((AchievementCategory)i);
            this->createCategoryButtons();
        });
        button->setID(buttonID);
        button->setEnabled(!isSelected);
        m_categoryMenu->addChild(button);
    }

    m_categoryMenu->setContentWidth(LIST_SIZE.width + 15.f);
    m_categoryMenu->setID("category-menu");
    m_categoryMenu->setLayout(AxisLayout::create(Axis::Row)->setGap(5.f));

    m_mainLayer->addChildAtPosition(m_categoryMenu, Anchor::Top, {0.f, -50.f});
}

void AchievementLayer::createFilterButtons() {
    cue::resetNode(m_filterMenu);
    m_filterMenu = CCMenu::create();

    for (int i = (int)FilterType::None; i < (int)FilterType::Last; i++) {
        auto filterType = (FilterType)i;
        bool isSelected = filterType == m_filterType;
        auto spriteName = getFilterSpriteName(filterType);

        auto spr = isSelected ? CCSprite::createWithSpriteFrameName(spriteName.c_str()) : CCSpriteGrayscale::createWithSpriteFrameName(spriteName);
        spr->setScale(0.6f);

        if (!isSelected) {
            spr->setColor({150, 150, 150});
            spr->setOpacity(215);
        }

        auto btn = Button::createWithNode(spr, [i, this](auto) {
            m_filterType = (FilterType)i;
            this->loadCategory(m_currentCategory);
            this->createFilterButtons();
        });

        if (isSelected) {
            btn->setEnabled(false);
        }

        btn->setID(filterTypeToNodeID(filterType) + "-button");
        m_filterMenu->addChild(btn);
    }

    m_filterMenu->setContentWidth(LIST_SIZE.width);
    m_filterMenu->setLayout(AxisLayout::create(Axis::Row)->setGap(2.f)->setAxisAlignment(AxisAlignment::End));
    m_filterMenu->setID("filter-menu");
    m_searchMenu->addChildAtPosition(m_filterMenu, Anchor::Center, {-10.f, 0.f});
}

bool AchievementLayer::matchSearch(std::shared_ptr<Achievement> ach, geode::ZStringView query) {
    bool addToList = false;
    double weighted = 0;

    addToList |= weightedFuzzyMatch(ach->identifier, query, 0.5, weighted);
    addToList |= weightedFuzzyMatch(ach->title, query, 1, weighted);
    
    if (weighted < 60.0 + 10.0 * query.size()) {
        addToList = false;
    }
    addToList |= weightedFuzzyMatch(ach->unachievedDescription, query, 0.5, weighted);
    addToList |= weightedFuzzyMatch(ach->achievedDescription, query, 0.5, weighted);

    return addToList;
}

}