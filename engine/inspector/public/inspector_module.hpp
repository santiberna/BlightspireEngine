#pragma once
#include "enum_utils.hpp"
#include "module_interface.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class Editor;
class ImGuiBackend;
class PerformanceTracker;
class ParticleEditor;

union SDL_Event;

enum class InputCaptureBits
{
    KEYBOARD = 1 << 0,
    MOUSE = 1 << 1,
};

class InspectorModule : public ModuleInterface
{
    ModuleTickOrder Init(Engine& engine) override;
    void Shutdown(Engine& engine) override;
    void Tick(Engine& engine) override;

public:
    InspectorModule();
    ~InspectorModule() override;

    NON_MOVABLE(InspectorModule);
    NON_COPYABLE(InspectorModule);

    bb::Flags<InputCaptureBits> ProcessInput(const SDL_Event& event);

private:
    std::unique_ptr<Editor> _editor;
    std::unique_ptr<PerformanceTracker> _performanceTracker;
    std::unique_ptr<ParticleEditor> _particleEditor;
    std::shared_ptr<ImGuiBackend> _imguiBackend;

    std::unordered_map<std::string, bool> _openWindows;
};
