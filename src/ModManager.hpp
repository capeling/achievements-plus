#pragma once

#include <asp/fs.hpp>

#include "singleton.hpp"
#include "Achievement.hpp"

namespace ap {

enum class AchievementCategory : uint8_t {
    None = 0,
    MainLevels,
    CompletedLevels,
    Stats,
    Coins,
    UserLevels,
    Social,
    Shards,
    Paths,
    Secrets,
    Last
};

std::string getAchievementCategoryName(AchievementCategory category);

class ModManager : public SingletonBase<ModManager> {
public:
    std::span<Achievement> getAchievementsInCategory(AchievementCategory category);

    // DEBUG
    void writeAchievementsToFile(asp::fs::Path path);
    void checkAllAchievements();

protected:
    std::array<std::vector<Achievement>, (int)AchievementCategory::Last> m_achievementCategories;
};

}