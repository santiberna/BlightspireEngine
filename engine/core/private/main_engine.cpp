#include "main_engine.hpp"

#include <string>
#include <tracy/Tracy.hpp>

int MainEngine::Run()
{
    while (!_exitRequested)
    {
        MainLoopOnce();
        FrameMark;
    }

    return _exitCode;
}
void MainEngine::MainLoopOnce()
{
    ZoneScoped;
    for (auto* entry : _tickOrder)
    {
        ZoneScoped;
        auto name = std::string(entry->name) + "::Tick";
        ZoneName(name.c_str(), name.size());

        entry->module->Tick(*this);

        if (_exitRequested)
        {
            return;
        }
    }
}

int MainEngine::GetExitCode() const
{
    return _exitCode;
}
