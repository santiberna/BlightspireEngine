#pragma once
#include "common.hpp"

#include <memory>

class ActionManager;
class UIElement;

struct UINavigationTargets
{
    std::weak_ptr<UIElement> up;
    std::weak_ptr<UIElement> down;
    std::weak_ptr<UIElement> left;
    std::weak_ptr<UIElement> right;
    std::weak_ptr<UIElement> forward;
    std::weak_ptr<UIElement> backward;
};

enum class UINavigationDirection : bb::u8
{
    eNone,
    eUp,
    eDown,
    eLeft,
    eRight,
    eForward,
    eBackward
};

[[nodiscard]] std::weak_ptr<UIElement> GetUINavigationTarget(const UINavigationTargets& navigationTargets, UINavigationDirection direction);
