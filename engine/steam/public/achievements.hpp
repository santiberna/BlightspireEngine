#pragma once
#include "common.hpp"
#include "steam_include.hpp"

#include <span>
#include <string>
#include <vector>

struct Achievement
{
    Achievement(bb::i32 id, const std::string_view& apiId)
        : id(id)
        , apiId(apiId)
        , name()
        , description()
        , achieved()
        , iconImage()
    {
    }
    Achievement() = default;

    bb::i32 id = 0;
    std::string apiId = "";
    char name[128];
    char description[256];
    bool achieved = false;
    bb::i32 iconImage = 0;
};

class SteamAchievementManager
{
private:
    bb::u64 _appID; // Our current AppID
    std::vector<Achievement> _achievements;
    bool _initialized; // Have we called Request stats and received the callback?

public:
    SteamAchievementManager(std::span<Achievement> achievements);
    ~SteamAchievementManager() = default;

    Achievement* GetAchievement(std::string_view name);

    bool SetAchievement(const char* ID);

    STEAM_CALLBACK(SteamAchievementManager, OnUserStatsReceived, UserStatsReceived_t,
        _callbackUserStatsReceived);
    STEAM_CALLBACK(SteamAchievementManager, OnUserStatsStored, UserStatsStored_t,
        _callbackUserStatsStored);
    STEAM_CALLBACK(SteamAchievementManager, OnAchievementStored,
        UserAchievementStored_t, _callbackAchievementStored);
};