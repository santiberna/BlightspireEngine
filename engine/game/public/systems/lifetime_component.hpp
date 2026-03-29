#pragma once

#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>


struct LifetimeComponent
{
    float lifetime = 0.f;
    bool paused = false;
};

namespace EnttEditor
{
template <>
void ComponentEditorWidget<LifetimeComponent>(entt::registry& reg, entt::registry::entity_type e);
}