#pragma once
#include "achievements.hpp"
#include "steam_stats.hpp"
#include <engine.hpp>
#include <memory>
#include <span>
#include <string>

class SteamModule : public ModuleInterface
{
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) override;

    void Tick([[maybe_unused]] Engine& engine) override;
    void Shutdown([[maybe_unused]] Engine& engine) override;

    bool _steamAvailable = false;
    bool _steamInputAvailable = false;
    float _statsCounterMs = 0;
    const float _statsCounterMaxMs = 5000;
    std::unique_ptr<SteamAchievementManager> _steamAchievements = nullptr;
    std::unique_ptr<SteamStatManager> _steamStats = nullptr;

public:
    NON_COPYABLE(SteamModule);
    NON_MOVABLE(SteamModule);

    SteamModule() = default;
    ~SteamModule() override = default;

    void InitSteamAchievements(std::span<Achievement> achievements);
    void InitSteamStats(std::span<Stat> stats);
    bool RequestCurrentStats();
    void SaveStats();

    SteamAchievementManager& GetAchievements() { return *_steamAchievements; }
    SteamStatManager& GetStats() { return *_steamStats; }

    // When the user launched the application through Steam, this will return true.
    // If false, the Steam module cannot be used, as Steam API does not work.
    bool Available() const { return _steamAvailable; }

    // Check first if steam is available before using.
    void OpenSteamBrowser(const std::string& url);

    // Returns whether Steam Input API is available to use.
    bool InputAvailable() const { return _steamInputAvailable; }
};
