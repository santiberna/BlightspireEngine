#pragma once
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>


// Add more variables for cheats here
struct CheatsComponent
{
    bool noClip = true;
};
namespace EnttEditor
{
template <>
void ComponentEditorWidget<CheatsComponent>(entt::registry& reg, entt::registry::entity_type e);
}