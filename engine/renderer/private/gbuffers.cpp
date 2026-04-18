#include "gbuffers.hpp"

#include "graphics_context.hpp"
#include "graphics_resources.hpp"
#include "resource_management/image_resource_manager.hpp"
#include "resource_management/sampler_resource_manager.hpp"
#include "vulkan_context.hpp"
#include "vulkan_helper.hpp"

GBuffers::GBuffers(const std::shared_ptr<GraphicsContext>& context, glm::uvec2 size)
    : _context(context)
    , _size(size)
{
    CreateGBuffers();
    CreateDepthResources();
    CreateViewportAndScissor();
}

GBuffers::~GBuffers()
{
    CleanUp();
}

void GBuffers::Resize(glm::uvec2 size)
{
    if (size == _size)
        return;

    CleanUp();

    _size = size;

    CreateGBuffers();
    CreateDepthResources();
    CreateViewportAndScissor();
}

void GBuffers::CreateGBuffers()
{
    auto resources { _context->Resources() };

    bb::Flags<bb::TextureFlags> flags;

    flags.set(bb::TextureFlags::COLOR_ATTACH);
    flags.set(bb::TextureFlags::SAMPLED);

    bb::Image2D g_buffer_spec;
    g_buffer_spec.width = _size.x;
    g_buffer_spec.height = _size.y;
    g_buffer_spec.format = bb::ImageFormat::R8G8B8A8_UNORM;

    _attachments[0] = resources->GetImageResourceManager().Create(g_buffer_spec, flags, "Albedo Metallic Roughness GBuffer");

    g_buffer_spec.format = bb::ImageFormat::R8G8_UNORM;

    _attachments[1] = resources->GetImageResourceManager().Create(g_buffer_spec, flags, "Normal GBuffer");
}

void GBuffers::CreateDepthResources()
{
    bb::SamplerCreation depthSampler {};
    depthSampler.name = "Nearest_Depth_Sampler";
    depthSampler.addressModeU = bb::SamplerAddressMode::CLAMP_TO_EDGE;
    depthSampler.addressModeV = bb::SamplerAddressMode::CLAMP_TO_EDGE;
    depthSampler.addressModeW = bb::SamplerAddressMode::CLAMP_TO_EDGE;

    depthSampler.minFilter = bb::SamplerFilter::NEAREST;
    depthSampler.magFilter = bb::SamplerFilter::NEAREST;
    depthSampler.mipmapMode = bb::SamplerFilter::NEAREST;

    depthSampler.useMaxAnisotropy = false;
    depthSampler.anisotropyEnable = false;
    depthSampler.minLod = 0.0f;
    depthSampler.maxLod = vk::LodClampNone;

    depthSampler.unnormalizedCoordinates = false;
    depthSampler.borderColor = bb::SamplerBorderColor::OPAQUE_BLACK_INT;
    _depthSampler = _context->Resources()->GetSamplerResourceManager().Create(depthSampler);

    bb::Flags<bb::TextureFlags> flags;

    flags.set(bb::TextureFlags::DEPTH_ATTACH);
    flags.set(bb::TextureFlags::SAMPLED);

    bb::Image2D g_buffer_spec;
    g_buffer_spec.width = _size.x;
    g_buffer_spec.height = _size.y;
    g_buffer_spec.format = bb::ImageFormat::D32_SFLOAT;

    _depthFormat = vk::Format::eD32Sfloat;
    _depthImage = _context->Resources()->GetImageResourceManager().Create(g_buffer_spec, flags, "Depth GBuffer");
}

void GBuffers::CleanUp()
{
}

void GBuffers::CreateViewportAndScissor()
{
    _viewport = vk::Viewport { 0.0f, 0.0f, static_cast<float>(_size.x), static_cast<float>(_size.y), 0.0f,
        1.0f };
    vk::Extent2D extent { _size.x, _size.y };

    _scissor = vk::Rect2D { vk::Offset2D { 0, 0 }, extent };
}

void GBuffers::TransitionLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    for (const auto& attachment : _attachments)
    {
        const GPUImage* image = _context->Resources()->GetImageResourceManager().Access(attachment);

        util::TransitionImageLayout(commandBuffer, image->handle, image->format, oldLayout, newLayout);
    }
}