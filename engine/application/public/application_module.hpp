#pragma once
#include "module_interface.hpp"
#include <glm/vec2.hpp>
#include <memory>
#include <string>

class InputDeviceManager;
class ActionManager;

struct SDL_Window;

class ApplicationModule : public ModuleInterface
{
    ModuleTickOrder Init(Engine& engine) override;
    void Shutdown(Engine& engine) override;
    void Tick(Engine& engine) override;

public:
    ApplicationModule();

    [[nodiscard]] SDL_Window* GetWindowHandle() const { return _window; }
    [[nodiscard]] InputDeviceManager& GetInputDeviceManager() const { return *_inputDeviceManager; }
    [[nodiscard]] ActionManager& GetActionManager() const { return *_actionManager; }

    [[nodiscard]] bool GetMouseHidden() const { return _mouseHidden; }
    void SetMouseHidden(bool val);

    void OpenExternalBrowser(const std::string& url);

    [[nodiscard]] glm::uvec2 DisplaySize() const;
    [[nodiscard]] bool isMinimized() const;

private:
    std::unique_ptr<InputDeviceManager> _inputDeviceManager {};
    std::unique_ptr<ActionManager> _actionManager {};
    SDL_Window* _window = nullptr;

    std::string _windowName = "Blightspire";
    bool _isFullscreen = true;
    bool _mouseHidden = true;
};