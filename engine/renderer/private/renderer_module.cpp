#include "renderer_module.hpp"

#include "animation_system.hpp"
#include "application_module.hpp"
#include "ecs_module.hpp"
#include "engine.hpp"
#include "graphics_context.hpp"
#include "particle_module.hpp"
#include "renderer.hpp"
#include "ui_module.hpp"
#include "vulkan_context.hpp"

#include <imgui.h>
#include <implot.h>
#include <memory>
#include <time_module.hpp>

RendererModule::RendererModule()
{
}

ModuleTickOrder RendererModule::Init(Engine& engine)
{
    auto& ecs = engine.GetModule<ECSModule>();
    auto& app = engine.GetModule<ApplicationModule>();

    VulkanInitInfo init_info {};
    init_info.window_size = app.DisplaySize();
    init_info.window_handle = app.GetWindowHandle();

    _context = std::make_shared<GraphicsContext>(init_info);
    _renderer = std::make_shared<Renderer>(engine.GetModule<ApplicationModule>(),
        engine.GetModule<UIModule>().GetViewport(), _context, ecs);

    ecs.AddSystem<AnimationSystem>();

    return ModuleTickOrder::eRender;
}

void RendererModule::Shutdown([[maybe_unused]] Engine& engine)
{
    _context->VulkanContext()->Device().waitIdle();
    _renderer.reset();

    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    _context.reset();
}

void RendererModule::Tick([[maybe_unused]] Engine& engine)
{
    auto dt = engine.GetModule<TimeModule>().GetDeltatime();
    _renderer->Render(dt.count());
}

std::vector<ResourceHandle<GPUModel>> RendererModule::LoadModels(const std::vector<CPUModel>& cpuModels)
{
    ZoneScopedN("Loading models into Renderer");
    auto result = _renderer->LoadModels(cpuModels);
    _renderer->UpdateBindless();

    return result;
}
