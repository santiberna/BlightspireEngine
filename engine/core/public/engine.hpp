#pragma once
#include "common.hpp"
#include "module_interface.hpp"

#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

// Service locator for all modules
// Instantiate a MainEngine to run the engine, which inherits from this
class Engine
{
public:
    Engine() = default;
    virtual ~Engine() { Reset(); }

    NON_COPYABLE(Engine);
    NON_MOVABLE(Engine);

    template <typename Module>
    Engine& AddModule();

    template <typename Module>
    Module& GetModule();

    template <typename Module>
    Module* TryGetModule();

    void RequestShutdown(int exit_code);

protected:
    struct ModuleEntry
    {
        ModuleInterface* module {};
        ModuleTickOrder ordering {};
        std::string name {};
    };

    int _exitCode = 0;
    bool _exitRequested = false;
    std::vector<ModuleEntry*> _tickOrder {};

    // Cleans up all modules
    void Reset();

private:
    [[nodiscard]] ModuleInterface* GetModuleUntyped(std::type_index type) const;
    void AddModuleToTickList(ModuleInterface* module, ModuleTickOrder priority);
    void RegisterNewModule(std::type_index module_type, ModuleInterface* module);

    // Raw pointers are used because deallocation order of modules is important

    std::unordered_map<std::type_index, ModuleEntry> _modules {};
    std::vector<ModuleEntry*> _initOrder {};
};

template <typename Module>
inline Engine& Engine::AddModule()
{
    GetModule<Module>();
    return *this;
}

template <typename Module>
inline Module& Engine::GetModule()
{
    if (auto modulePtr = TryGetModule<Module>())
    {
        return static_cast<Module&>(*modulePtr);
    }

    auto type = std::type_index(typeid(Module));
    auto* new_module = new Module();

    RegisterNewModule(type, new_module);
    return *new_module;
}

template <typename Module>
Module* Engine::TryGetModule()
{
    return static_cast<Module*>(GetModuleUntyped(std::type_index(typeid(Module))));
}