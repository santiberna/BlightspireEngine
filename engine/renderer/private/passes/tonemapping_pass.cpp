#include "passes/tonemapping_pass.hpp"

#include "bloom_settings.hpp"
#include "gpu_scene.hpp"
#include "graphics_context.hpp"
#include "graphics_resources.hpp"
#include "pipeline_builder.hpp"
#include "resource_management/image_resource_manager.hpp"
#include "resources/buffer.hpp"
#include "settings.hpp"
#include "shaders/shader_loader.hpp"
#include "vulkan_context.hpp"
#include "vulkan_helper.hpp"

TonemappingPass::TonemappingPass(const std::shared_ptr<GraphicsContext>& context, const Settings::Tonemapping& settings, ResourceHandle<GPUImage> hdrTarget, ResourceHandle<GPUImage> bloomTarget, ResourceHandle<GPUImage> volumetricTarget, ResourceHandle<GPUImage> outputTarget, const SwapChain& _swapChain, const GBuffers& gBuffers, const BloomSettings& bloomSettings)
    : _context(context)
    , _settings(settings)
    , _swapChain(_swapChain)
    , _gBuffers(gBuffers)
    , _hdrTarget(hdrTarget)
    , _bloomTarget(bloomTarget)
    , _volumetricTarget(volumetricTarget)
    , _outputTarget(outputTarget)
    , _bloomSettings(bloomSettings)
{
    CreateSettingsDescriptorSetLayout(_context);
    CreateSettingsBuffers();
    CreateSettingsDescriptorSets();
    
    CreatePaletteBuffer();
    CreateDescriptorSetLayouts();
    CreateDescriptorSets();
    CreatePipeline();

    _tonemappingSettings.hdrTargetIndex = hdrTarget.getIndex();
    _tonemappingSettings.bloomTargetIndex = bloomTarget.getIndex();
    _tonemappingSettings.depthIndex = gBuffers.Depth().getIndex();
    _tonemappingSettings.volumetricIndex = volumetricTarget.getIndex();
    _tonemappingSettings.screenWidth = _gBuffers.Size().x;
    _tonemappingSettings.screenHeight = _gBuffers.Size().y;
    _tonemappingSettings.normalRIndex = _gBuffers.Attachments()[1].getIndex();
}

TonemappingPass::~TonemappingPass()
{
    vk::Device device = _context->GetVulkanContext()->Device();
    device.destroy(_pipeline);
    device.destroy(_pipelineLayout);
    device.destroy(_paletteDescriptorSetLayout);
    _context->Resources()->GetBufferResourceManager().Destroy(_colorPaletteBuffer);
    device.destroy(_settingsDescriptorSetLayout);
}

void TonemappingPass::RecordCommands(vk::CommandBuffer commandBuffer, bb::u32 currentFrame, [[maybe_unused]] const RenderSceneDescription& scene)
{
    TracyVkZone(scene.tracyContext, commandBuffer, "Tonemapping Pass");

    timePassed += scene.deltaTime / 1000.0f;
    _tonemappingSettings.exposure = _settings.exposure;
    _tonemappingSettings.tonemappingFunction = static_cast<bb::u32>(_settings.tonemappingFunction);
    _tonemappingSettings.rayOrigin.a -= (0.2 * (scene.deltaTime / 1000.0f));

    _tonemappingSettings.enableFlags = 0;

    if (_settings.enableVignette)
    {
        _tonemappingSettings.enableFlags |= eEnableVignette;
    }

    if (_settings.enableLensDistortion)
    {
        _tonemappingSettings.enableFlags |= eEnableLensDistortion;
    }

    if (_settings.enableToneAdjustments)
    {
        _tonemappingSettings.enableFlags |= eEnableToneAdjustments;
    }

    if (_settings.enablePixelization)
    {
        _tonemappingSettings.enableFlags |= eEnablePixelization;
    }

    if (_settings.enablePalette)
    {
        _tonemappingSettings.enableFlags |= eEnablePalette;
    }

    _tonemappingSettings.vignetteIntensity = _settings.vignetteIntensity;
    _tonemappingSettings.lensDistortionIntensity = _settings.lensDistortionIntensity;
    _tonemappingSettings.lensDistortionCubicIntensity = _settings.lensDistortionCubicIntensity;
    _tonemappingSettings.screenScale = _settings.screenScale;

    _tonemappingSettings.brightness = _settings.brightness;
    _tonemappingSettings.contrast = _settings.contrast;
    _tonemappingSettings.saturation = _settings.saturation;
    _tonemappingSettings.vibrance = _settings.vibrance;
    _tonemappingSettings.hue = _settings.hue;

    float scaleFactor = _gBuffers.Size().y / 1080.0f;
    _tonemappingSettings.minPixelSize = _settings.minPixelSize * scaleFactor;
    _tonemappingSettings.maxPixelSize = _settings.maxPixelSize * scaleFactor;
    _tonemappingSettings.pixelizationLevels = _settings.pixelizationLevels;
    _tonemappingSettings.pixelizationDepthBias = _settings.pixelizationDepthBias;
    _tonemappingSettings.ditherAmount = _settings.ditherAmount;
    _tonemappingSettings.paletteAmount = _settings.paletteAmount;
    _tonemappingSettings.time = timePassed;

    _tonemappingSettings.skyColor = _settings.skyColor;
    _tonemappingSettings.sunColor = _settings.sunColor;
    _tonemappingSettings.cloudsColor = _settings.cloudsColor;
    _tonemappingSettings.voidColor = _settings.voidColor;
    _tonemappingSettings.cloudsSpeed = _settings.cloudsSpeed;
    _tonemappingSettings.waterColor = _settings.waterColor;

    _tonemappingSettings.paletteSize = static_cast<int>(_settings.palette.size());

    const bb::Buffer* buffer = _context->Resources()->GetBufferResourceManager().Access(_settingsBuffers[currentFrame]);
    std::memcpy(buffer->mappedPtr, &_tonemappingSettings, sizeof(_tonemappingSettings));
    
    UpdatePaletteBuffer(_settings.palette);

    vk::RenderingAttachmentInfoKHR finalColorAttachmentInfo {
        .imageView = _context->Resources()->GetImageResourceManager().Access(_outputTarget)->view,
        .imageLayout = vk::ImageLayout::eAttachmentOptimalKHR,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = {
            .color = vk::ClearColorValue { .float32 = { { 0.0f, 0.0f, 0.0f, 0.0f } } } },
    };

    vk::RenderingInfoKHR renderingInfo {
        .renderArea = {
            .offset = vk::Offset2D { 0, 0 },
            .extent = _swapChain.GetExtent(),
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &finalColorAttachmentInfo,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
    };

    commandBuffer.beginRenderingKHR(&renderingInfo);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);

    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipelineLayout, 0, { _context->BindlessSet() }, {});
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipelineLayout, 1, { _bloomSettings.GetDescriptorSetData(currentFrame) }, {});
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipelineLayout, 2, { scene.gpuScene->MainCamera().DescriptorSet(currentFrame) }, {});
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipelineLayout, 3, { scene.gpuScene->GetSceneDescriptorSet(currentFrame) }, {});
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipelineLayout, 4, { _paletteDescriptorSet }, {});
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipelineLayout, 5, { _settingsDescriptorSets[currentFrame] }, {});

    // Fullscreen triangle.
    commandBuffer.draw(3, 1, 0, 0);

    _context->GetDrawStats().Draw(3);

    commandBuffer.endRenderingKHR();
}

void TonemappingPass::CreatePipeline()
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

    std::vector<std::byte> vertSpv = shader::ReadFile("shaders/bin/fullscreen.vert.spv");
    std::vector<std::byte> fragSpv = shader::ReadFile("shaders/bin/tonemapping.frag.spv");

    GraphicsPipelineBuilder pipelineBuilder { _context };
    pipelineBuilder.AddShaderStage(vk::ShaderStageFlagBits::eVertex, vertSpv);
    pipelineBuilder.AddShaderStage(vk::ShaderStageFlagBits::eFragment, fragSpv);
    auto result = pipelineBuilder
                      .SetColorBlendState(colorBlendStateCreateInfo)
                      .SetColorAttachmentFormats({ _context->Resources()->GetImageResourceManager().Access(_outputTarget)->format })
                      .SetDepthAttachmentFormat(vk::Format::eUndefined)
                      .BuildPipeline();

    _pipelineLayout = std::get<0>(result);
    _pipeline = std::get<1>(result);
}
void TonemappingPass::CreatePaletteBuffer()
{
    std::vector<glm::vec4> colors(_maxColorsInPalette);
    vk::DeviceSize bufferSize = sizeof(glm::vec4) * _maxColorsInPalette;

    bb::Flags<bb::BufferFlags> flags = { bb::BufferFlags::UNIFORM_USAGE, bb::BufferFlags::TRANSFER_DST, bb::BufferFlags::MAPPABLE };
    _colorPaletteBuffer = _context->Resources()->GetBufferResourceManager().Create(bufferSize, flags, "Palette Buffer");

    // Immediately update the buffer with the initial palette data.
    UpdatePaletteBuffer(colors);
}
void TonemappingPass::CreateDescriptorSetLayouts()
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings = {
        vk::DescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eAll,
            .pImmutableSamplers = nullptr }
    };

    _paletteDescriptorSetLayout = PipelineBuilder::CacheDescriptorSetLayout(*_context->GetVulkanContext(), bindings, { "ColorPaletteUBO" });
}

void TonemappingPass::UpdatePaletteBuffer(const std::vector<glm::vec4>& paletteColors)
{
    // Get a pointer to the mapped buffer memory.
    void* data = _context->Resources()->GetBufferResourceManager().Access(_colorPaletteBuffer)->mappedPtr;
    memcpy(data, paletteColors.data(), paletteColors.size() * sizeof(glm::vec4));
}

void TonemappingPass::CreateDescriptorSets()
{
    vk::DescriptorSetAllocateInfo allocInfo {
        .descriptorPool = _context->GetVulkanContext()->DescriptorPool(),
        .descriptorSetCount = 1,
        .pSetLayouts = &_paletteDescriptorSetLayout
    };

    vk::Device device = _context->GetVulkanContext()->Device();
    if (device.allocateDescriptorSets(&allocInfo, &_paletteDescriptorSet) != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to allocate descriptor set");
    }

    vk::DescriptorBufferInfo colorPaletteBufferInfo {
        .buffer = _context->Resources()->GetBufferResourceManager().Access(_colorPaletteBuffer)->buffer,
        .offset = 0,
        .range = vk::WholeSize,
    };

    std::array<vk::WriteDescriptorSet, 1> descriptorWrites = {
        vk::WriteDescriptorSet {
            .dstSet = _paletteDescriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pBufferInfo = &colorPaletteBufferInfo }
    };

    device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void TonemappingPass::CreateSettingsDescriptorSetLayout(const std::shared_ptr<GraphicsContext>& context)
{
    vk::DescriptorSetLayoutBinding descriptorSetBinding {
        .binding = 0,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1,
        .stageFlags = vk::ShaderStageFlagBits::eAllGraphics | vk::ShaderStageFlagBits::eCompute,
    };

    std::vector<vk::DescriptorSetLayoutBinding> bindings { descriptorSetBinding };
    std::vector<std::string_view> names { "SettingsUBO" };

    _settingsDescriptorSetLayout = PipelineBuilder::CacheDescriptorSetLayout(*context->GetVulkanContext(), bindings, names);
}

void TonemappingPass::CreateSettingsBuffers()
{
    vk::DeviceSize bufferSize = sizeof(GPUCamera);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        std::string name = "[] Tonemapping Settings Buffer";

        // Inserts i in the middle of []
        name.insert(1, 1, static_cast<char>(i + '0'));

        bb::Flags<bb::BufferFlags> flags = { bb::BufferFlags::UNIFORM_USAGE, bb::BufferFlags::MAPPABLE };
        _settingsBuffers.at(i) = _context->Resources()->GetBufferResourceManager().Create(bufferSize, flags, name.c_str());
    }
}

void TonemappingPass::CreateSettingsDescriptorSets()
{
    vk::Device device = _context->GetVulkanContext()->Device();

    std::array<vk::DescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> layouts {};
    std::for_each(layouts.begin(), layouts.end(), [this](auto& l)
        { l = _settingsDescriptorSetLayout; });
    vk::DescriptorSetAllocateInfo allocateInfo {};
    allocateInfo.descriptorPool = _context->GetVulkanContext()->DescriptorPool();
    allocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
    allocateInfo.pSetLayouts = layouts.data();

    util::VK_ASSERT(device.allocateDescriptorSets(&allocateInfo, _settingsDescriptorSets.data()),
        "Failed allocating descriptor sets!");

    for (size_t i = 0; i < _settingsDescriptorSets.size(); ++i)
    {
        const bb::Buffer* buffer = _context->Resources()->GetBufferResourceManager().Access(_settingsBuffers[i]);

        vk::DescriptorBufferInfo bufferInfo {
            .buffer = buffer->buffer,
            .offset = 0,
            .range = vk::WholeSize,
        };

        vk::WriteDescriptorSet bufferWrite {
            .dstSet = _settingsDescriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pBufferInfo = &bufferInfo,
        };
        device.updateDescriptorSets({ bufferWrite }, {});
    }
}
