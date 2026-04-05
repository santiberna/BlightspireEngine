#pragma once
#include "common.hpp"

#include "module_interface.hpp"
#include "time.hpp"

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

    [[nodiscard]] bb::MillisecondsF32 GetDeltatime() const { return { _currentDeltaTime.value * _deltaTimeScale }; }
    [[nodiscard]] bb::MillisecondsF32 GetRealDeltatime() const { return _currentDeltaTime; }
    [[nodiscard]] bb::MillisecondsF32 GetTotalTime() const { return _totalTime; }

    void SetDeltatimeScale(float scale)
    {
        _deltaTimeScale = scale;
    }

    void ResetTimer()
    {
        _deltaTimer = {};
        _currentDeltaTime = {};
    }

private:
    float _deltaTimeScale = 1.0f;

    bb::MillisecondsF32 _currentDeltaTime {};
    bb::MillisecondsF32 _totalTime {};

    bb::Stopwatch _deltaTimer {};
};