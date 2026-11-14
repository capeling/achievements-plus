#include "ModManager.hpp"

using namespace geode::prelude;

namespace ap {

std::string getAchievementCategoryName(AchievementCategory category) {
    switch (category) {
        using enum AchievementCategory;
        case MainLevels:
            return "Main Levels";
        case CompletedLevels:
            return "Levels";
        case Stats:
            return "Stats";
        case Coins:
            return "Coins";
        case UserLevels:
            return "User Levels";
        case Social:
            return "Social";
        case Shards:
            return "Shards";
        case Paths:
            return "Paths";
        case Secrets:
            return "Other";
        case None:
        case Last:
        default:
            break;
    }
    return "Unknown";
}

std::span<std::shared_ptr<Achievement>> ModManager::getAchievementsInCategory(AchievementCategory category) {
    auto& vec = m_achievementCategories[(int)category];
        if (!vec.empty())
            return vec;
     
    auto am = AchievementManager::sharedState();
    auto allAchievementDicts = am->m_allAchievementsSorted->asExt();

    auto addRangeToVec = [&allAchievementDicts, &vec](size_t from, size_t to) {
        if (from > to) {
            std::swap(from, to);
        }

        for (int i = from; i < to; i++) {
            auto achDict = static_cast<CCDictionary*>(allAchievementDicts[i]);
            if (!achDict)
                return;
            auto ach = Achievement::fromCCDict(achDict);
            vec.emplace_back(ach);
        }
    };

    auto addToVec = [&am, &vec](ZStringView id) {
        auto achDict = am->getAchievementsWithID(id.c_str());
        if (!achDict)
            return;
        vec.emplace_back(Achievement::fromCCDict(achDict));
    };

    switch (category) {
        using enum AchievementCategory;
        case None:
        case Last:
            break;
        case MainLevels:
            addRangeToVec(0, 52);
            addToVec("geometry.ach.demoncoin01");
            addToVec("geometry.ach.demoncoin02");
            addToVec("geometry.ach.demoncoin03");
            addToVec("geometry.ach.steam01");
            addToVec("geometry.ach.steam02");
            break;
        case CompletedLevels:
            addRangeToVec(52, 89);
            addRangeToVec(282, 307);
            break;
        case Stats:
            addRangeToVec(92, 151);
            addRangeToVec(240, 258);
            break;
        case Coins:
            addRangeToVec(151, 219);
            break;
        case UserLevels:
            addRangeToVec(219, 240);
            addRangeToVec(258, 265);
            addRangeToVec(267, 281);
            addRangeToVec(327, 334);
            addToVec("geometry.ach.hyped01");
            break;
        case Social:
            addToVec("geometry.ach.follower01");
            addRangeToVec(307, 327);
            break;
        case Shards:
            addRangeToVec(379, 439);
            break;
        case Paths:
            addRangeToVec(439, 549);
            break;
        case Secrets:
            addRangeToVec(334, 377);
            addToVec("geometry.ach.rate");
            break;
        default:
            break;
    }
    return vec;
}

void ModManager::checkAllAchievements() {
    auto am = AchievementManager::sharedState();
    CCArrayExt<CCDictionary*> allAchievementDicts = am->m_allAchievementsSorted->asExt();

    for (auto& achDict : allAchievementDicts) {
        bool found = false;
        auto identifier = static_cast<CCString*>(achDict->objectForKey("identifier"))->getCString();
        for (int i = (int)AchievementCategory::None + 1; i < (int)AchievementCategory::Last; i++) {
            auto span = getAchievementsInCategory((AchievementCategory)i);
            auto it = std::ranges::find_if(span, [identifier](const ModManager::AchievementPtr& ach) {
                return std::string(identifier) == ach->identifier;
            });
            if (it != span.end()) {
                found = true;
                break;
            }
        }

        if (!found)
            log::error("MISSING ACHIEVEMENT \"{}\"", identifier);
    }

    log::info("Finished checking all achievements");
}

void ModManager::writeAchievementsToFile(asp::fs::Path path) {
    if (!path.exists()) {
        auto thing = asp::fs::File::create(path);
        if (!thing)
            return;
        this->writeAchievementsToFile(path);
        return;
    }

    if (!path.isFile())
        return;

    auto am = AchievementManager::sharedState();
    auto allAchievementDicts = am->m_allAchievementsSorted->asExt();

    auto openOptions = asp::fs::OpenOptions{};
    openOptions.write = true;
    auto file = asp::fs::File::open(path, openOptions);
    if (!file)
        return;

    auto fuw = std::move(file.unwrap());

    for (auto& ach : allAchievementDicts) {
        auto acha = Achievement::fromCCDict(static_cast<CCDictionary*>(ach));
        auto str = fmt::format("\"{}\": \"{}\", \"{}\"\n", acha->identifier, acha->title, acha->unachievedDescription);
        auto a = fuw.write(str);
    }

    fuw.close();
}

}