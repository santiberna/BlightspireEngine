#include "engine.hpp"

#include <algorithm>
#include <array>
#include <tracy/Tracy.hpp>

namespace
{
// Could be moved to utility, but currently only used here
std::string getTypeName(std::type_index index)
{
    constexpr std::array<std::string_view, 3> TRIM_STARTS = { "struct ", "class ", "enum " };
    std::string_view name = index.name();

    for (const auto& start : TRIM_STARTS)
    {
        if (name.starts_with(start))
        {
            return std::string(name.substr(start.size()));
        }
    }
    return std::string(name);
}
}

void Engine::RequestShutdown(int exit_code)
{
    _exitRequested = true;
    _exitCode = exit_code;
}

Engine::~Engine()
{
    for (auto it = _initOrder.rbegin(); it != _initOrder.rend(); ++it)
    {
        auto* entry = *it;

        ZoneScoped;
        auto zone_name = entry->name + "::Shutdown";
        ZoneName(zone_name.c_str(), zone_name.size());

        auto* module = entry->module;
        module->Shutdown(*this);
        delete module;
    }
}

ModuleInterface* Engine::GetModuleUntyped(std::type_index type) const
{
    if (auto it = _modules.find(type); it != _modules.end())
    {
        return it->second.module;
    }
    else
    {
        return nullptr;
    }
}

void Engine::RegisterNewModule(std::type_index module_type, ModuleInterface* module)
{
    ZoneScoped;
    auto module_name = getTypeName(module_type);
    auto zone_name = module_name + "::Init";
    ZoneName(zone_name.c_str(), zone_name.size());

    ModuleEntry new_entry {};
    new_entry.module = module;
    new_entry.name = std::move(module_name);

    auto [it, success] = _modules.emplace(module_type, std::move(new_entry));
    auto ordering = it->second.module->Init(*this);

    // Iterator can invalidate after Init(), which is why this is looked up again
    ModuleEntry* entry = &_modules.find(module_type)->second;
    entry->ordering = ordering;

    // Sorted emplace, based on tick ordering
    auto compare = [](const ModuleEntry* new_elem, const ModuleEntry* old_elem)
    {
        return new_elem->ordering < old_elem->ordering;
    };

    auto insert_it = std::ranges::upper_bound(_tickOrder, entry, compare);

    _tickOrder.insert(insert_it, entry);
    _initOrder.emplace_back(entry);
}