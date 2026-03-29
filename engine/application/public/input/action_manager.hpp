#pragma once
#include "common.hpp"

#include "enum_utils.hpp"
#include "input/input_codes/gamepad.hpp"
#include "input/input_codes/keys.hpp"
#include "input/input_codes/mousebuttons.hpp"

#include <glm/vec2.hpp>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class InputDeviceManager;

enum class DigitalActionType : uint8_t
{
    eNone = 0 << 0,
    // Action happens once when input is received.
    ePressed = 1 << 0,
    // Action happens continuously when input is being received.
    eHeld = 1 << 1,
    // Action happens once when input is released.
    eReleased = 1 << 2,
};
GENERATE_ENUM_FLAG_OPERATORS(DigitalActionType)

struct KeyboardAnalog
{
    KeyboardCode up;
    KeyboardCode down;
    KeyboardCode left;
    KeyboardCode right;
};

struct MouseAnalog
{
};

using DigitalInputBinding = std::variant<GamepadButton, KeyboardCode, MouseButton>;
using AnalogInputBinding = std::variant<GamepadAnalog, KeyboardAnalog, MouseAnalog>;

// Action for button inputs.
struct DigitalAction
{
    std::string name {};
    std::vector<DigitalInputBinding> inputs {};
};

// Action for axis inputs.
struct AnalogAction
{
    std::string name {};
    std::vector<AnalogInputBinding> inputs {};
};

// A collection of actions for a game mode. (game mode -> how a player should play in a situation, e.g. main menu or gameplay)
struct ActionSet
{
    std::string name {};
    std::vector<DigitalAction> digitalActions {};
    std::vector<AnalogAction> analogActions {};
};

// A collection of action sets that contain all actions across the game.
using GameActions = std::vector<ActionSet>;

// Utility wrapper for easy action state checking.
struct DigitalActionResult
{
    // True once upon pressed.
    [[nodiscard]] bool IsPressed() const { return HasAnyFlags(value, DigitalActionType::ePressed); }
    // True all the time upon held.
    [[nodiscard]] bool IsHeld() const { return HasAnyFlags(value, DigitalActionType::eHeld); }
    // True once upon released.
    [[nodiscard]] bool IsReleased() const { return HasAnyFlags(value, DigitalActionType::eReleased); }

    DigitalActionType value = DigitalActionType::eNone;
};

struct GamepadTypeGlyphs
{
    std::unordered_map<GamepadButton, std::string> digitals {};
    std::unordered_map<GamepadAnalog, std::string> analogs {};
};

using GamepadGlyphs = std::unordered_map<GamepadType, GamepadTypeGlyphs>;

struct BindingOriginVisual
{
    std::string bindingInputName {};
    // The path to the glyph file used to load a texture. May be empty if a glyph is not available for the binding.
    std::string glyphImagePath {};
};

// Abstract class, which manages keyboard and mouse actions.
// Inherit to handle gamepad actions.
class ActionManager
{
public:
    ActionManager(const InputDeviceManager& inputDeviceManager);
    virtual ~ActionManager() = default;

    NON_COPYABLE(ActionManager);
    NON_MOVABLE(ActionManager);

    virtual void Update() { }

    // Sets the actions in the game, the first set in the game actions is assumed to be the used set.
    virtual void SetGameActions(const GameActions& gameActions) { _gameActions = gameActions; }
    // Change the currently used action set in the game actions.
    virtual void SetActiveActionSet(std::string_view actionSetName);
    // Sets custom gamepad glyph paths to be taken into account
    void SetCustomInputGlyphs(const GamepadGlyphs& gamepadGlyphs) { _gamepadGlyphs = gamepadGlyphs; }

    // Button actions.
    [[nodiscard]] DigitalActionResult GetDigitalAction(std::string_view actionName) const;

    // Axis actions.
    [[nodiscard]] glm::vec2 GetAnalogAction(std::string_view actionName) const;

    // Returns information to be visually displayed for all bindings for the given digital action.
    [[nodiscard]] std::vector<BindingOriginVisual> GetDigitalActionBindingOriginVisual(std::string_view actionName) const;

    // Returns information to be visually displayed for all bindings for the given analog action.
    [[nodiscard]] std::vector<BindingOriginVisual> GetAnalogActionBindingOriginVisual(std::string_view actionName) const;

protected:
    const InputDeviceManager& _inputDeviceManager;
    GameActions _gameActions {};
    uint32_t _activeActionSet = 0;
    GamepadGlyphs _gamepadGlyphs {};

    [[nodiscard]] DigitalActionType CheckDigitalInput(const DigitalAction& action) const;
    [[nodiscard]] DigitalActionType CheckInput([[maybe_unused]] std::string_view actionName, KeyboardCode code) const;
    [[nodiscard]] DigitalActionType CheckInput([[maybe_unused]] std::string_view actionName, MouseButton button) const;
    [[nodiscard]] virtual DigitalActionType CheckInput(std::string_view actionName, GamepadButton button) const = 0;
    [[nodiscard]] glm::vec2 CheckAnalogInput(const AnalogAction& action) const;
    [[nodiscard]] glm::vec2 CheckInput([[maybe_unused]] std::string_view actionName, const KeyboardAnalog& keyboardAnalog) const;
    [[nodiscard]] glm::vec2 CheckInput([[maybe_unused]] std::string_view actionName, [[maybe_unused]] const MouseAnalog& mouseAnalog) const;
    [[nodiscard]] virtual glm::vec2 CheckInput(std::string_view actionName, GamepadAnalog gamepadAnalog) const = 0;
    [[nodiscard]] virtual std::vector<BindingOriginVisual> GetDigitalActionGamepadOriginVisual(const DigitalAction& action) const;
    [[nodiscard]] virtual std::vector<BindingOriginVisual> GetAnalogActionGamepadOriginVisual(const AnalogAction& action) const;
    [[nodiscard]] std::vector<BindingOriginVisual> GetDigitalMouseAndKeyboardOriginVisual(const DigitalAction& action) const;
    [[nodiscard]] std::vector<BindingOriginVisual> GetAnalogMouseAndKeyboardOriginVisual(const AnalogAction& action) const;
};
