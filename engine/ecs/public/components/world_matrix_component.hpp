#pragma once
#include <glm/mat4x4.hpp>
#include <imgui_entt_entity_editor.hpp>

struct WorldMatrixComponent
{
private:
    glm::mat4 _worldMatrix { 1.0f };
    friend class TransformHelpers;

public:
    void Inspect();
};

namespace EnttEditor
{
template <>
void ComponentEditorWidget<WorldMatrixComponent>(entt::registry& reg, entt::registry::entity_type e);
}