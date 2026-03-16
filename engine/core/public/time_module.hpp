#pragma once
#include "engine.hpp"
#include "timers.hpp"

class TimeModule : public ModuleInterface
{
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) override;
    void Tick([[maybe_unused]] Engine& engine) override;
    void Shutdown([[maybe_unused]] Engine& engine) override { };

public:
    NON_COPYABLE(TimeModule);
    NON_MOVABLE(TimeModule);

    TimeModule() = default;
    ~TimeModule() override = default;
    DeltaMS GetDeltatime() const { return _currentDeltaTime * _deltaTimeScale; }
    DeltaMS GetRealDeltatime() const { return _currentDeltaTime; }
    DeltaMS GetTotalTime() const { return _totalTime; }

    void SetDeltatimeScale(float scale)
    {
        _deltaTimeScale = scale;
    }

    void ResetTimer()
    {
        _deltaTimer.Reset();
        _currentDeltaTime = {};
    }

private:
    float _deltaTimeScale = 1.0f;

    DeltaMS _currentDeltaTime {};
    DeltaMS _totalTime {};

    Stopwatch _deltaTimer {};
};