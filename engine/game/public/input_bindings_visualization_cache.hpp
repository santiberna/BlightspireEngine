#pragma once

#include "graphics_context.hpp"
#include "input/action_manager.hpp"

struct GPUImage;

struct CachedBindingOriginVisual
{
    std::string bindingInputName {};
    // Texture for the input visual. May be empty if a glyph is not available for the binding.
    ResourceHandle<GPUImage> glyphImage {};
};

class InputBindingsVisualizationCache
{
public:
    InputBindingsVisualizationCache(const ActionManager& actionManager, GraphicsContext& graphicsContext);

    [[nodiscard]] std::vector<CachedBindingOriginVisual> GetDigital(std::string_view actionName);
    [[nodiscard]] std::vector<CachedBindingOriginVisual> GetAnalog(std::string_view actionName);

private:
    [[nodiscard]] ResourceHandle<GPUImage> GetGlyph(const std::string& path);

    const ActionManager& _actionManager;
    GraphicsContext& _graphicsContext;
    std::unordered_map<std::string, ResourceHandle<GPUImage>> _glyphCache {};
};
