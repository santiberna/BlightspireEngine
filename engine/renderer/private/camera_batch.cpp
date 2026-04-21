#include "camera_batch.hpp"

#include "gpu_scene.hpp"
#include "graphics_context.hpp"
#include "graphics_resources.hpp"
#include "math.hpp"
#include "pipeline_builder.hpp"
#include "resource_management/buffer_resource_manager.hpp"
#include "resource_management/image_resource_manager.hpp"
#include "resource_management/sampler_resource_manager.hpp"
#include "vulkan_context.hpp"

CameraBatch::Draw::Draw(const std::shared_ptr<GraphicsContext>& context, const std::string& name, uint32_t instanceCount, vk::DescriptorSetLayout drawDSL, vk::DescriptorSetLayout visibilityDSL, vk::DescriptorSetLayout redirectDSL)
{
    auto& buffers = context->Resources()->GetBufferResourceManager();

    std::string drawBuf = name + " draw buffer";
    std::string redirectBuf = name + " redirect buffer";
    std::string visBuf = name + " visibility buffer";

    bb::Flags<bb::BufferFlags> flagsDrawBuf = { bb::BufferFlags::STORAGE_USAGE, bb::BufferFlags::INDIRECT_USAGE };
    bb::Flags<bb::BufferFlags> flagsRedirectBuf = { bb::BufferFlags::STORAGE_USAGE, bb::BufferFlags::INDIRECT_USAGE, bb::BufferFlags::TRANSFER_DST };
    bb::Flags<bb::BufferFlags> flagsVisBuf = { bb::BufferFlags::STORAGE_USAGE };

    drawBuffer = buffers.Create(instanceCount * sizeof(DrawIndexedIndirectCommand), flagsDrawBuf, drawBuf.c_str());
    redirectBuffer = buffers.Create(sizeof(uint32_t) + (instanceCount * sizeof(uint32_t)), flagsRedirectBuf, redirectBuf.c_str());
    visibilityBuffer = buffers.Create(instanceCount / 8, flagsVisBuf, visBuf.c_str());

    drawDescriptor = CreateDescriptor(context, drawDSL, drawBuffer);
    redirectDescriptor = CreateDescriptor(context, redirectDSL, redirectBuffer);
    visibilityDescriptor = CreateDescriptor(context, visibilityDSL, visibilityBuffer);
}

vk::DescriptorSet CameraBatch::Draw::CreateDescriptor(const std::shared_ptr<GraphicsContext>& context, vk::DescriptorSetLayout dsl, ResourceHandle<bb::Buffer> buffer)
{
    vk::DescriptorSetAllocateInfo allocateInfo {
        .descriptorPool = context->GetVulkanContext()->DescriptorPool(),
        .descriptorSetCount = 1,
        .pSetLayouts = &dsl,
    };

    vk::Device device = context->GetVulkanContext()->Device();
    vk::DescriptorSet descriptor = device.allocateDescriptorSets(allocateInfo).value.front();

    vk::DescriptorBufferInfo bufferInfo {
        .buffer = context->Resources()->GetBufferResourceManager().Access(buffer)->buffer,
        .offset = 0,
        .range = vk::WholeSize,
    };

    vk::WriteDescriptorSet bufferWrite {
        .dstSet = descriptor,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eStorageBuffer,
        .pBufferInfo = &bufferInfo,
    };

    device.updateDescriptorSets({ bufferWrite }, {});
    return descriptor;
}

CameraBatch::CameraBatch(const std::shared_ptr<GraphicsContext>& context, const std::string& name, const CameraResource& camera, ResourceHandle<GPUImage> depthImage, vk::DescriptorSetLayout drawDSL, vk::DescriptorSetLayout visibilityDSL, vk::DescriptorSetLayout redirectDSL)
    : _context(context)
    , _camera(camera)
    , _depthImage(depthImage)
    , _staticDraw(context, "Static " + name, MAX_STATIC_INSTANCES, drawDSL, visibilityDSL, redirectDSL)
    , _skinnedDraw(context, "Skinned" + name, MAX_SKINNED_INSTANCES, drawDSL, visibilityDSL, redirectDSL)
{
    const auto* depthImageAccess = _context->Resources()->GetImageResourceManager().get(_depthImage);
    uint16_t hzbSize = math::RoundUpToPowerOfTwo(std::max(depthImageAccess->width, depthImageAccess->height));

    bb::SamplerCreation samplerCreation {};
    samplerCreation.name = name + " HZB Sampler";
    samplerCreation.minFilter = bb::SamplerFilter::LINEAR;
    samplerCreation.magFilter = bb::SamplerFilter::LINEAR;
    samplerCreation.anisotropyEnable = false;
    samplerCreation.borderColor = bb::SamplerBorderColor::OPAQUE_BLACK_INT;
    samplerCreation.mipmapMode = bb::SamplerFilter::LINEAR;
    samplerCreation.minLod = 0.0f;
    samplerCreation.maxLod = static_cast<float>(std::floor(std::log2(hzbSize)));
    samplerCreation.reductionMode = _camera.UsesReverseZ() ? bb::SamplerReductionMode::MIN : bb::SamplerReductionMode::MAX;
    samplerCreation.addressModeU = bb::SamplerAddressMode::CLAMP_TO_BORDER;
    samplerCreation.addressModeW = bb::SamplerAddressMode::CLAMP_TO_BORDER;
    samplerCreation.addressModeV = bb::SamplerAddressMode::CLAMP_TO_BORDER;

    _hzbSampler = _context->Resources()->GetSamplerResourceManager().Create(samplerCreation);

    bb::Image2D hzb_image {};
    hzb_image.width = hzbSize;
    hzb_image.height = hzbSize;
    hzb_image.format = bb::ImageFormat::R32_SFLOAT;

    bb::Flags<bb::TextureFlags> flags {};
    flags.set(bb::TextureFlags::SAMPLED);
    flags.set(bb::TextureFlags::GEN_MIPMAPS);
    flags.set(bb::TextureFlags::STORAGE_ACCESS);

    _hzbImage = _context->Resources()->GetImageResourceManager().Create(hzb_image, _hzbSampler, flags, name + " HZB Image", nullptr);
}

CameraBatch::~CameraBatch() = default;
