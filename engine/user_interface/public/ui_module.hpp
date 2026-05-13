#pragma once
#include "engine.hpp"
#include "ui_input.hpp"
#include "viewport.hpp"

class GraphicsContext;
class UIModule : public ModuleInterface
{
public:
    UIModule() = default;
    ~UIModule() override = default;

    NON_COPYABLE(UIModule);
    NON_MOVABLE(UIModule);

    [[nodiscard]] Viewport& GetViewport() { return *_viewport; };
    [[nodiscard]] const Viewport& GetViewport() const { return *_viewport; };

    UIInputContext uiInputContext;

private:
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) final;

    std::unique_ptr<Viewport> _viewport;
    std::shared_ptr<GraphicsContext> _graphicsContext;

    void Tick([[maybe_unused]] Engine& engine) final;
    void Shutdown([[maybe_unused]] Engine& engine) final { }
};
