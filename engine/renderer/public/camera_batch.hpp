#pragma once

#include "resource_manager.hpp"
#include "vulkan_fwd.hpp"

#include <string>

class GPUScene;
class GraphicsContext;
struct GPUImage;
struct Sampler;
class CameraResource;

namespace bb
{
struct Buffer;
}

class CameraBatch
{
public:
    struct Draw
    {
        Draw(const std::shared_ptr<GraphicsContext>& context, const std::string& name, uint32_t instanceCount, VkDescriptorSetLayout drawDSL, VkDescriptorSetLayout visibilityDSL, VkDescriptorSetLayout redirectDSL);

        ResourceHandle<bb::Buffer> drawBuffer;
        ResourceHandle<bb::Buffer> redirectBuffer;
        ResourceHandle<bb::Buffer> visibilityBuffer;
        VkDescriptorSet drawDescriptor;
        VkDescriptorSet redirectDescriptor;
        VkDescriptorSet visibilityDescriptor;

    private:
        VkDescriptorSet CreateDescriptor(const std::shared_ptr<GraphicsContext>& context, VkDescriptorSetLayout dsl, ResourceHandle<bb::Buffer> buffer);
    };

    CameraBatch(const std::shared_ptr<GraphicsContext>& context, const std::string& name, const CameraResource& camera, ResourceHandle<GPUImage> depthImage, VkDescriptorSetLayout drawDSL, VkDescriptorSetLayout visibilityDSL, VkDescriptorSetLayout redirectDSL);
    ~CameraBatch();

    const CameraResource& Camera() const { return _camera; }
    ResourceHandle<GPUImage> HZBImage() const { return _hzbImage; }
    ResourceHandle<GPUImage> DepthImage() const { return _depthImage; }
    Draw StaticDraw() const { return _staticDraw; }
    Draw SkinnedDraw() const { return _skinnedDraw; }

private:
    std::shared_ptr<GraphicsContext> _context;

    const CameraResource& _camera;

    ResourceHandle<GPUImage> _hzbImage;
    ResourceHandle<Sampler> _hzbSampler;

    ResourceHandle<GPUImage> _depthImage;

    Draw _staticDraw;
    Draw _skinnedDraw;
};
