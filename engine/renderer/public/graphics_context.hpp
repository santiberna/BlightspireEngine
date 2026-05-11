#pragma once

#include "common.hpp"
#include "draw_stats.hpp"
#include "resource_manager.hpp"
#include "vulkan_fwd.hpp"

#include <memory>

class VulkanContext;
class GraphicsResources;
struct VulkanInitInfo;

struct GPUImage;
struct SDL_Window;

namespace bb
{
struct Buffer;
}

constexpr bb::u32 MAX_BINDLESS_RESOURCES = 1024;

class GraphicsContext
{
public:
    GraphicsContext(SDL_Window* window);
    ~GraphicsContext();

    NON_COPYABLE(GraphicsContext);
    NON_MOVABLE(GraphicsContext);

    [[nodiscard]] std::shared_ptr<VulkanContext> GetVulkanContext() const { return _vulkanContext; }
    [[nodiscard]] std::shared_ptr<GraphicsResources> Resources() const { return _graphicsResources; }
    [[nodiscard]] VkDescriptorSetLayout BindlessLayout() const;
    [[nodiscard]] VkDescriptorSet BindlessSet() const;

    DrawStats& GetDrawStats() { return _drawStats; }
    [[nodiscard]] const ResourceHandle<GPUImage>& FallbackImage() const { return _fallbackImage; }

    void UpdateBindlessSet();

private:
    std::shared_ptr<VulkanContext> _vulkanContext;
    std::shared_ptr<GraphicsResources> _graphicsResources;

    ResourceHandle<bb::Sampler> _sampler;
    ResourceHandle<GPUImage> _fallbackImage;

    struct BindlessObjects;
    std::unique_ptr<BindlessObjects> bindless {};

    DrawStats _drawStats;

    void CreateBindlessDescriptorSet();
    void CreateBindlessMaterialBuffer();

    void UpdateBindlessImages();
    void UpdateBindlessMaterials();
};
