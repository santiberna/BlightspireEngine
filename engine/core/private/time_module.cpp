#include "time_module.hpp"

ModuleTickOrder TimeModule::Init([[maybe_unused]] Engine& e)
{
    return ModuleTickOrder::eFirst;
}

void TimeModule::Tick([[maybe_unused]] Engine& e)
{
    auto nanos = _deltaTimer.getElapsed();
    _currentDeltaTime = bb::durationCast<bb::MillisecondsF32>(nanos);
    _deltaTimer = {};
    _totalTime.value += _currentDeltaTime.value;
}