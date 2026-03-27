#include "input_bindings_visualization_cache.hpp"
#include "graphics_resources.hpp"
#include "resource_management/image_resource_manager.hpp"

InputBindingsVisualizationCache::InputBindingsVisualizationCache(const ActionManager& actionManager, GraphicsContext& graphicsContext)
    : _actionManager(actionManager)
    , _graphicsContext(graphicsContext)
{
}

std::vector<CachedBindingOriginVisual> InputBindingsVisualizationCache::GetDigital(std::string_view actionName)
{
    auto visualizations = _actionManager.GetDigitalActionBindingOriginVisual(actionName);
    std::vector<CachedBindingOriginVisual> out {};

    for (const auto& visualization : visualizations)
    {
        CachedBindingOriginVisual& cached = out.emplace_back();
        cached.bindingInputName = visualization.bindingInputName;
        cached.glyphImage = GetGlyph(visualization.glyphImagePath);
    }

    return out;
}

std::vector<CachedBindingOriginVisual> InputBindingsVisualizationCache::GetAnalog(std::string_view actionName)
{
    auto visualizations = _actionManager.GetAnalogActionBindingOriginVisual(actionName);
    std::vector<CachedBindingOriginVisual> out {};

    for (const auto& visualization : visualizations)
    {
        CachedBindingOriginVisual& cached = out.emplace_back();
        cached.bindingInputName = visualization.bindingInputName;
        cached.glyphImage = GetGlyph(visualization.glyphImagePath);
    }

    return out;
}

ResourceHandle<GPUImage> InputBindingsVisualizationCache::GetGlyph(const std::string& path)
{
    if (path.empty())
    {
        return ResourceHandle<GPUImage>::Null();
    }

    auto it = _glyphCache.find(path);
    if (it != _glyphCache.end())
    {
        return it->second;
    }

    auto glyphImage = bb::Image2D::fromFile(path);

    if (!glyphImage.has_value())
    {
        return ResourceHandle<GPUImage>::Null();
    }

    auto& imageResourceManager = _graphicsContext.Resources()->ImageResourceManager();

    SingleTimeCommands upload { *_graphicsContext.GetVulkanContext() };
    GPUImage texture { upload, glyphImage.value(), imageResourceManager._defaultSampler, bb::TextureFlags::COMMON_FLAGS, nullptr };
    auto image = imageResourceManager.Create(std::move(texture));

    _graphicsContext.UpdateBindlessSet();
    _glyphCache.emplace(path, image);
    return image;
}
