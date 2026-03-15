#pragma once
#include "input/action_manager.hpp"
#include "steam_include.hpp"

class SteamInputDeviceManager;

class SteamActionManager final : public ActionManager
{
public:
    SteamActionManager(const SteamInputDeviceManager& steamInputDeviceManager);
    ~SteamActionManager() final = default;

    void Update() final;
    void SetGameActions(const GameActions& gameActions) final;
    void SetActiveActionSet(std::string_view actionSetName) final;

private:
    const SteamInputDeviceManager& _steamInputDeviceManager;

    struct SteamActionSetCache
    {
        InputActionSetHandle_t actionSetHandle {};
        std::unordered_map<std::string, InputDigitalActionHandle_t> gamepadDigitalActionsCache {};
        std::unordered_map<std::string, InputDigitalActionHandle_t> gamepadAnalogActionsCache {};
    };

    using SteamGameActionsCache = std::vector<SteamActionSetCache>;
    SteamGameActionsCache _steamGameActionsCache {};

    // We have to track pressed and released inputs ourselves as steam input API doesn't do it for us properly,
    // so we save the current and previous frames input states per action.
    using SteamControllerState = std::unordered_map<std::string, bool>;
    SteamControllerState _currentControllerState {};
    SteamControllerState _prevControllerState {};

    [[nodiscard]] DigitalActionType CheckInput(std::string_view actionName, [[maybe_unused]] GamepadButton button) const;
    [[nodiscard]] glm::vec2 CheckInput(std::string_view actionName, [[maybe_unused]] GamepadAnalog gamepadAnalog) const final;
    [[nodiscard]] std::vector<BindingOriginVisual> GetDigitalActionGamepadOriginVisual(const DigitalAction& action) const final;
    [[nodiscard]] std::vector<BindingOriginVisual> GetAnalogActionGamepadOriginVisual(const AnalogAction& action) const final;
    void UpdateSteamControllerInputState();
    void CacheSteamInputHandles();
};
