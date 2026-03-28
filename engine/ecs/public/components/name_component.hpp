#pragma once

#include <imgui_entt_entity_editor.hpp>
#include <string>

struct NameComponent
{
public:
    std::string name = "Unnamed Entity";
    static std::string_view GetDisplayName(const entt::registry& registry, entt::entity entity);
};

namespace EnttEditor
{
template <>
void ComponentEditorWidget<NameComponent>(entt::registry& reg, entt::registry::entity_type e);
}