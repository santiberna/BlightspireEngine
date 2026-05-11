#include "components/camera_component.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace EnttEditor
{
template <>
void ComponentEditorWidget<CameraComponent>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& t = reg.get<CameraComponent>(e);
    t.Inspect();
}
}
void CameraComponent::Inspect()
{
    bb::i32 currentProjection = static_cast<bb::i32>(projection);
    constexpr bb::i32 projectionElementCount = static_cast<bb::i32>(Projection::eCount);
    const char* projectionElementNames[projectionElementCount] = { "Perspective", "Orthographic" };
    const char* currentProjectionName = "Unknown";

    if (currentProjection >= 0 && currentProjection < projectionElementCount)
    {
        currentProjectionName = projectionElementNames[currentProjection];
    }
    else
    {
        spdlog::warn("CameraComponent", "Invalid projection type: {}", currentProjection);
    }

    ImGui::SliderInt("Projection##Camera", &currentProjection, 0, projectionElementCount - 1, currentProjectionName);

    ImGui::SliderAngle("Field of View##Camera", &fov, 1.0f);
    ImGui::DragFloat("Near Plane##Camera", &nearPlane, 0.1f);
    ImGui::DragFloat("Far Plane##Camera", &farPlane, 0.1f);
}