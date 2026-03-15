#include "main_engine.hpp"

#include <string>
#include <tracy/Tracy.hpp>

int* MainEngine::Tick()
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
            return &_exitCode;
        }
    }
    FrameMark;
    return nullptr;
}
