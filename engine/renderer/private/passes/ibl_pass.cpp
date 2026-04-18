#include "passes/ibl_pass.hpp"

#include "graphics_context.hpp"
#include "graphics_resources.hpp"
#include "pipeline_builder.hpp"
#include "resource_management/image_resource_manager.hpp"
#include "resource_management/sampler_resource_manager.hpp"
#include "shaders/shader_loader.hpp"
#include "single_time_commands.hpp"
#include "vulkan_context.hpp"
#include "vulkan_helper.hpp"

IBLPass::IBLPass(const std::shared_ptr<GraphicsContext>& context, ResourceHandle<GPUImage> environmentMap)
    : _context(context)
    , _environmentMap(environmentMap)
{
    bb::SamplerCreation createInfo {
        .name = "IBL sampler",
        .maxLod = 6.0f,
    };

    createInfo.addressModeU = bb::SamplerAddressMode::CLAMP_TO_EDGE;
    createInfo.addressModeW = bb::SamplerAddressMode::CLAMP_TO_EDGE;
    createInfo.addressModeV = bb::SamplerAddressMode::CLAMP_TO_EDGE;

    _sampler = _context->Resources()->GetSamplerResourceManager().Create(createInfo);

    CreateIrradianceCubemap();
    CreatePrefilterCubemap();
    CreateBRDFLUT();
    CreateIrradiancePipeline();
    CreatePrefilterPipeline();
    CreateBRDFLUTPipeline();
}

IBLPass::~IBLPass()
{
    vk::Device device = _context->GetVulkanContext()->Device();

    device.destroy(_prefilterPipeline);
    device.destroy(_prefilterPipelineLayout);
    device.destroy(_irradiancePipeline);
    device.destroy(_irradiancePipelineLayout);
    device.destroy(_brdfLUTPipeline);
    device.destroy(_brdfLUTPipelineLayout);
}

void IBLPass::RecordCommands(vk::CommandBuffer commandBuffer)
{
    const GPUImage& irradianceMap = *_context->Resources()->GetImageResourceManager().get(_irradianceMap);
    const GPUImage& prefilterMap = *_context->Resources()->GetImageResourceManager().get(_prefilterMap);

    util::BeginLabel(commandBuffer, "Irradiance pass", glm::vec3 { 17.0f, 138.0f, 178.0f } / 255.0f);

    util::TransitionImageLayout(commandBuffer, irradianceMap.handle, irradianceMap.format, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, 6, 0, 1);

    for (size_t i = 0; i < 6; ++i)
    {
        vk::RenderingAttachmentInfoKHR finalColorAttachmentInfo {
            .imageView = irradianceMap.layerViews[i].view,
            .imageLayout = vk::ImageLayout::eAttachmentOptimal,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eStore,
        };

        vk::RenderingInfoKHR renderingInfo {
            .renderArea = {
                .offset = vk::Offset2D { 0, 0 },
                .extent = vk::Extent2D { static_cast<uint32_t>(irradianceMap.width), static_cast<uint32_t>(irradianceMap.height) },
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &finalColorAttachmentInfo,
            .pDepthAttachment = nullptr,
            .pStencilAttachment = nullptr,
        };

        commandBuffer.beginRenderingKHR(&renderingInfo);

        IrradiancePushConstant pc {
            .index = static_cast<uint32_t>(i),
            .hdriIndex = _environmentMap.getIndex(),
        };

        commandBuffer.pushConstants<IrradiancePushConstant>(_irradiancePipelineLayout, vk::ShaderStageFlagBits::eFragment, 0, { pc });
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _irradiancePipeline);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _irradiancePipelineLayout, 0, { _context->BindlessSet() }, {});

        vk::Viewport viewport = vk::Viewport { 0.0f, 0.0f, static_cast<float>(irradianceMap.width), static_cast<float>(irradianceMap.height), 0.0f,
            1.0f };
        commandBuffer.setViewport(0, 1, &viewport);
        commandBuffer.setScissor(0, { renderingInfo.renderArea });

        commandBuffer.draw(3, 1, 0, 0);

        _context->GetDrawStats().Draw(3);

        commandBuffer.endRenderingKHR();
    }

    util::TransitionImageLayout(commandBuffer, irradianceMap.handle, irradianceMap.format, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 6, 0, 1);
    util::EndLabel(commandBuffer);

    util::BeginLabel(commandBuffer, "Prefilter pass", glm::vec3 { 17.0f, 138.0f, 178.0f } / 255.0f);
    util::TransitionImageLayout(commandBuffer, prefilterMap.handle, prefilterMap.format, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, 6, 0, prefilterMap.mips);

    auto& layers = prefilterMap.layerViews;

    for (size_t i = 0; i < prefilterMap.mips; ++i)
    {
        for (size_t j = 0; j < 6; ++j)
        {
            vk::RenderingAttachmentInfoKHR finalColorAttachmentInfo {
                .imageView = layers[j].mipViews[i],
                .imageLayout = vk::ImageLayout::eAttachmentOptimal,
                .loadOp = vk::AttachmentLoadOp::eLoad,
                .storeOp = vk::AttachmentStoreOp::eStore,
            };
            uint32_t size = static_cast<uint32_t>(prefilterMap.width >> i);

            vk::RenderingInfoKHR renderingInfo {
                .renderArea = {
                    .offset = vk::Offset2D { 0, 0 },
                    .extent = vk::Extent2D { size, size },
                },
                .layerCount = 1,
                .colorAttachmentCount = 1,
                .pColorAttachments = &finalColorAttachmentInfo,
                .pDepthAttachment = nullptr,
                .pStencilAttachment = nullptr,
            };

            commandBuffer.beginRenderingKHR(&renderingInfo);

            PrefilterPushConstant pc {
                .faceIndex = static_cast<uint32_t>(j),
                .roughness = static_cast<float>(i) / static_cast<float>(prefilterMap.mips - 1),
                .hdriIndex = _environmentMap.getIndex(),
            };

            commandBuffer.pushConstants<PrefilterPushConstant>(_prefilterPipelineLayout, vk::ShaderStageFlagBits::eFragment, 0, { pc });
            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _prefilterPipeline);
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _prefilterPipelineLayout, 0, { _context->BindlessSet() }, {});

            vk::Viewport viewport = vk::Viewport { 0.0f, 0.0f, static_cast<float>(size), static_cast<float>(size), 0.0f,
                1.0f };
            commandBuffer.setViewport(0, 1, &viewport);
            commandBuffer.setScissor(0, { renderingInfo.renderArea });

            commandBuffer.draw(3, 1, 0, 0);

            _context->GetDrawStats().Draw(3);

            commandBuffer.endRenderingKHR();
        }
    }

    util::TransitionImageLayout(commandBuffer, prefilterMap.handle, prefilterMap.format, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 6, 0, prefilterMap.mips);

    util::EndLabel(commandBuffer);

    const GPUImage* brdfLUT = _context->Resources()->GetImageResourceManager().get(_brdfLUT);
    util::BeginLabel(commandBuffer, "BRDF Integration pass", glm::vec3 { 17.0f, 138.0f, 178.0f } / 255.0f);
    util::TransitionImageLayout(commandBuffer, brdfLUT->handle, brdfLUT->format, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);

    vk::RenderingAttachmentInfoKHR finalColorAttachmentInfo {
        .imageView = _context->Resources()->GetImageResourceManager().get(_brdfLUT)->view,
        .imageLayout = vk::ImageLayout::eAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
    };

    vk::RenderingInfoKHR renderingInfo {
        .renderArea = {
            .offset = vk::Offset2D { 0, 0 },
            .extent = vk::Extent2D { brdfLUT->width, brdfLUT->height },
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &finalColorAttachmentInfo,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
    };

    commandBuffer.beginRenderingKHR(&renderingInfo);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _brdfLUTPipeline);

    vk::Viewport viewport = vk::Viewport { 0.0f, 0.0f, static_cast<float>(brdfLUT->width), static_cast<float>(brdfLUT->height), 0.0f,
        1.0f };

    commandBuffer.setViewport(0, { viewport });
    commandBuffer.setScissor(0, { renderingInfo.renderArea });

    commandBuffer.draw(3, 1, 0, 0);

    _context->GetDrawStats().Draw(3);

    commandBuffer.endRenderingKHR();

    util::TransitionImageLayout(commandBuffer, brdfLUT->handle, brdfLUT->format, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    util::EndLabel(commandBuffer);
}

void IBLPass::CreateIrradiancePipeline()
{
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState {
        .blendEnable = vk::False,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
    };

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo {
        .logicOpEnable = vk::False,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
    };

    std::vector<vk::Format> formats { _context->Resources()->GetImageResourceManager().get(_irradianceMap)->format };

    std::vector<std::byte> vertSpv = shader::ReadFile("shaders/bin/fullscreen.vert.spv");
    std::vector<std::byte> fragSpv = shader::ReadFile("shaders/bin/irradiance.frag.spv");

    GraphicsPipelineBuilder pipelineBuilder { _context };
    pipelineBuilder.AddShaderStage(vk::ShaderStageFlagBits::eVertex, vertSpv);
    pipelineBuilder.AddShaderStage(vk::ShaderStageFlagBits::eFragment, fragSpv);
    auto result = pipelineBuilder
                      .SetColorBlendState(colorBlendStateCreateInfo)
                      .SetColorAttachmentFormats(formats)
                      .BuildPipeline();

    _irradiancePipelineLayout = std::get<0>(result);
    _irradiancePipeline = std::get<1>(result);
}

void IBLPass::CreatePrefilterPipeline()
{
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState {
        .blendEnable = vk::False,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
    };

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo {
        .logicOpEnable = vk::False,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
    };

    std::vector<vk::Format> formats { _context->Resources()->GetImageResourceManager().get(_prefilterMap)->format };

    std::vector<std::byte> vertSpv = shader::ReadFile("shaders/bin/fullscreen.vert.spv");
    std::vector<std::byte> fragSpv = shader::ReadFile("shaders/bin/prefilter.frag.spv");

    GraphicsPipelineBuilder pipelineBuilder { _context };
    pipelineBuilder.AddShaderStage(vk::ShaderStageFlagBits::eVertex, vertSpv);
    pipelineBuilder.AddShaderStage(vk::ShaderStageFlagBits::eFragment, fragSpv);
    auto result = pipelineBuilder
                      .SetColorBlendState(colorBlendStateCreateInfo)
                      .SetColorAttachmentFormats(formats)
                      .BuildPipeline();

    _prefilterPipelineLayout = std::get<0>(result);
    _prefilterPipeline = std::get<1>(result);
}

void IBLPass::CreateBRDFLUTPipeline()
{
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState {
        .blendEnable = vk::False,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
    };

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo {
        .logicOpEnable = vk::False,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
    };

    std::vector<vk::Format> formats { _context->Resources()->GetImageResourceManager().get(_brdfLUT)->format };

    std::vector<std::byte> vertSpv = shader::ReadFile("shaders/bin/fullscreen.vert.spv");
    std::vector<std::byte> fragSpv = shader::ReadFile("shaders/bin/brdf_integration.frag.spv");

    GraphicsPipelineBuilder pipelineBuilder { _context };
    pipelineBuilder.AddShaderStage(vk::ShaderStageFlagBits::eVertex, vertSpv);
    pipelineBuilder.AddShaderStage(vk::ShaderStageFlagBits::eFragment, fragSpv);
    auto result = pipelineBuilder
                      .SetColorBlendState(colorBlendStateCreateInfo)
                      .SetColorAttachmentFormats(formats)
                      .BuildPipeline();

    _brdfLUTPipelineLayout = std::get<0>(result);
    _brdfLUTPipeline = std::get<1>(result);
}

void IBLPass::CreateIrradianceCubemap()
{
    bb::Cubemap cubemap {};
    cubemap.height = 32;
    cubemap.width = 32;
    cubemap.format = bb::ImageFormat::R16G16B16A16_SFLOAT;

    auto& textures = _context->Resources()->GetImageResourceManager();
    _irradianceMap = textures.Create(
        cubemap, textures._defaultSampler, { bb::TextureFlags::SAMPLED, bb::TextureFlags::COLOR_ATTACH }, "Irradiance Cubemap");
}

void IBLPass::CreatePrefilterCubemap()
{
    bb::Cubemap cubemap {};
    cubemap.height = 128;
    cubemap.width = 128;
    cubemap.format = bb::ImageFormat::R16G16B16A16_SFLOAT;

    auto& textures = _context->Resources()->GetImageResourceManager();
    bb::Flags<bb::TextureFlags> flags = { bb::TextureFlags::SAMPLED, bb::TextureFlags::COLOR_ATTACH, bb::TextureFlags::GEN_MIPMAPS };

    _prefilterMap = textures.Create(
        cubemap, textures._defaultSampler, flags, "Prefilter Cubemap");
}

void IBLPass::CreateBRDFLUT()
{
    bb::Image2D image {};
    image.width = 512;
    image.height = 512;
    image.format = bb::ImageFormat::R16G16_SFLOAT;

    _brdfLUT = _context->Resources()->GetImageResourceManager().Create(
        image, { bb::TextureFlags::COLOR_ATTACH, bb::TextureFlags::SAMPLED }, "BRDF LUT");
}
