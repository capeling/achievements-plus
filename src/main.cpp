#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "ui/AchievementLayer.hpp"

using namespace geode::prelude;

// ItzKiba is poopy

class $modify(MenuLayer) {
	void onAchievements(CCObject* sender) {
#ifdef DEBUG_BUILD
		auto dp = CCKeyboardDispatcher::get();
		auto& mm = ap::ModManager::get();
		if (dp->getAltKeyPressed()) {
			mm.checkAllAchievements();
		} else if (dp->getShiftKeyPressed()) {
			mm.writeAchievementsToFile(Mod::get()->getConfigDir() / "achievements.txt");
		} else if (dp->getControlKeyPressed()) {
			auto l = AchievementsLayer::create();
			this->addChild(l, 100);
			l->showLayer(false);
			return;
		}
#endif

		ap::AchievementLayer::create()->show();
	}
};