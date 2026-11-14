#include "AchievementLayer.hpp"

#include "AchievementCell.hpp"

using namespace geode::prelude;

namespace ap {

std::string getFilterSpriteName(FilterType type) {
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

    // auto testBtn = Button::createWithLabel("Vanilla", "bigFont.fnt", [](auto) {
    //     auto layer = AchievementsLayer::create();
    //     CCScene::get()->addChild(layer, 5000);
    //     layer->showLayer(false);
    // });
    // testBtn->setScale(0.6f);
    // m_mainLayer->addChildAtPosition(testBtn, Anchor::Bottom, {0, 20.f});

    m_achievementList = cue::ListNode::create(LIST_SIZE);
    m_achievementList->setCellColors<ccColor3B, ccColor3B>({161, 88, 44}, {194, 114, 62});
    m_mainLayer->addChildAtPosition(m_achievementList, Anchor::Center, {0, -15.f});

    this->loadCategory(AchievementCategory::MainLevels);
    this->createCategoryButtons();
    this->createFilterButtons();

    m_title->setPositionY(m_title->getPositionY() + 5.f);

    return true;
}

void AchievementLayer::loadCategory(AchievementCategory category) {
    m_currentCategory = category;
    m_achievementList->clear();

    cue::resetNode(m_countLabel);

    auto& mm = ModManager::get();
    auto am = AchievementManager::sharedState();

    auto createCountLabel = [this](int amount) {
        auto am = AchievementManager::sharedState();
        m_countLabel = Label::create(fmt::format("{}/{} Achievements", amount, am->m_allAchievementsSorted->count()), "goldFont.fnt");
        m_countLabel->setScale(0.4f);
        auto winSize = CCDirector::get()->getWinSize();
        this->addChild(m_countLabel);
        m_countLabel->setPosition(winSize.width - m_countLabel->getScaledContentWidth() / 2.f - 5.f, winSize.height - m_countLabel->getScaledContentHeight() + 2.5f);
    };

    auto achs = mm.getAchievementsInCategory(category);
    if (m_filterType == FilterType::None) {
        for (auto& ach : achs) {
            m_achievementList->addCell(AchievementCell::create(ach, {LIST_SIZE.width, 45.f}));
        }
        this->setTitle(getAchievementCategoryName(category));
        createCountLabel(achs.size());
        return;
    }

    auto achsSorted = std::vector<ModManager::AchievementPtr>(achs.begin(), achs.end());
    switch (m_filterType) {
        using enum FilterType;
        case Progress:
            std::sort(achsSorted.begin(), achsSorted.end(), [](const ModManager::AchievementPtr& a, const ModManager::AchievementPtr& b) {
                auto am = AchievementManager::sharedState();
                auto ap = am->percentForAchievement(a->identifier.c_str());
                auto bp = am->percentForAchievement(b->identifier.c_str());

                return ap > bp;
            });
            break;
        case Uncompleted:
            for (auto it = achsSorted.begin(); it != achsSorted.end(); ) {
                auto obj = *it;
                if (am->isAchievementEarned(obj->identifier.c_str())) {
                    it = achsSorted.erase(it);
                    continue;
                }
                ++it;
            }
            break;
        case Completed:
            for (auto it = achsSorted.begin(); it != achsSorted.end(); ) {
                auto obj = *it;
                if (!am->isAchievementEarned(obj->identifier.c_str())) {
                    it = achsSorted.erase(it);
                    continue;
                }
                ++it;
            }
            break;
        default:
            break;
    }

    for (auto& ach : achsSorted) {
        m_achievementList->addCell(AchievementCell::create(ach, {LIST_SIZE.width, 45.f}));
    }

    createCountLabel(achsSorted.size());
    this->setTitle(getAchievementCategoryName(category));
}

void AchievementLayer::createCategoryButtons() {
    cue::resetNode(m_categoryMenu);
    m_categoryMenu = CCMenu::create();
    for (int i = (int)AchievementCategory::None + 1; i < (int)AchievementCategory::Last; i++) {
        auto name = getAchievementCategoryName((AchievementCategory)i);
        name = utils::string::replace(name, " ", "\n");

        bool isSelected = (AchievementCategory)i == m_currentCategory;

        auto btnSprite = ButtonSprite::create(name.c_str(), 58.f, 0.f, 0.266f, false, "bigFont.fnt", isSelected ? "GJ_button_02.png" : "GJ_button_05.png", 25.f);
        auto button = Button::createWithNode(btnSprite, [this, i](auto) {
            this->loadCategory((AchievementCategory)i);
            this->createCategoryButtons();
        });
        button->setEnabled(!isSelected);
        m_categoryMenu->addChild(button);
    }

    m_categoryMenu->setContentWidth(LIST_SIZE.width + 15.f);
    m_categoryMenu->setLayout(AxisLayout::create(Axis::Row)->setGap(5.f));

    m_mainLayer->addChildAtPosition(m_categoryMenu, Anchor::Top, {0.f, -40.f});
}

void AchievementLayer::createFilterButtons() {
    cue::resetNode(m_filterMenu);
    m_filterMenu = CCMenu::create();

    for (int i = (int)FilterType::None; i < (int)FilterType::Last; i++) {
        auto filterType = (FilterType)i;
        bool isSelected = filterType == m_filterType;
        auto spriteName = getFilterSpriteName(filterType);

        auto spr = isSelected ? CCSprite::createWithSpriteFrameName(spriteName.c_str()) : CCSpriteGrayscale::createWithSpriteFrameName(spriteName);
        spr->setScale(0.9f);

        auto btn = Button::createWithNode(spr, [i, this](auto) {
            m_filterType = (FilterType)i;
            this->loadCategory(m_currentCategory);
            this->createFilterButtons();
        });
        m_filterMenu->addChild(btn);
    }

    m_filterMenu->setLayout(AxisLayout::create(Axis::Column)->setAxisReverse(true));
    m_mainLayer->addChildAtPosition(m_filterMenu, Anchor::Left, {22.5f, -15.f});
}

}