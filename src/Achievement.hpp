#pragma once

namespace ap {

struct Achievement {
    struct Reward {
        static Reward fromString(geode::ZStringView string);

        UnlockType type = UnlockType::Cube;
        int id = 0;
    };

    static std::shared_ptr<Achievement> fromCCDict(cocos2d::CCDictionary* dict);

    std::string identifier = "";
    std::string title = "";
    Reward reward;
    std::string unachievedDescription = "";
    std::string achievedDescription = "";
};

}

template <>
struct fmt::formatter<ap::Achievement::Reward> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const ap::Achievement::Reward& p, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "(type: {}, id: {})", 
            (int)p.type,
            p.id
        );
    }
};

template <>
struct fmt::formatter<ap::Achievement> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const ap::Achievement& p, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "(identifier: {}, title: {}, reward: {}, unachievedDescription: {}, achievedDescription: {})", 
            p.identifier,
            p.title,
            p.reward,
            p.unachievedDescription,
            p.achievedDescription
        );
    }
};