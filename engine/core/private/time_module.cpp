#include "time_module.hpp"

ModuleTickOrder TimeModule::Init([[maybe_unused]] Engine& e)
{
    return ModuleTickOrder::eFirst;
}

void TimeModule::Tick([[maybe_unused]] Engine& e)
{
    _currentDeltaTime = _deltaTimer.GetElapsed();
    _deltaTimer.Reset();
    _totalTime += _currentDeltaTime;
}