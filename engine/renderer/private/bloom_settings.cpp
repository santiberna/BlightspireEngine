#include "bloom_settings.hpp"

#include "graphics_context.hpp"
#include "graphics_resources.hpp"
#include "pipeline_builder.hpp"
#include "resource_management/buffer_resource_manager.hpp"
#include "settings.hpp"
#include "vulkan_helper.hpp"

BloomSettings::BloomSettings(const std::shared_ptr<GraphicsContext>& context, const Settings::Bloom& settings)
    : _context(context)
    , _settings(settings)
{
    CreateDescriptorSetLayout();
    CreateUniformBuffers();
}

BloomSettings::~BloomSettings()
{
    auto vkContext { _context->GetVulkanContext() };
    auto resources { _context->Resources() };

    vk::Device device = vkContext->Device();
    device.destroy(_descriptorSetLayout);
}

void BloomSettings::Update(uint32_t currentFrame)
{
    _data.strength = _settings.strength;
    _data.colorWeights = _settings.colorWeights;
    _data.gradientStrength = _settings.gradientStrength;
    _data.maxBrightnessExtraction = _settings.maxBrightnessExtraction;
    _data.filterRadius = _settings.filterRadius;

    auto resources { _context->Resources() };

    const bb::Buffer* buffer = resources->GetBufferResourceManager().Access(_frameData.buffers.at(currentFrame));
    memcpy(buffer->mappedPtr, &_data, sizeof(SettingsData));
}

void BloomSettings::CreateDescriptorSetLayout()
{
    auto vkContext { _context->GetVulkanContext() };

    std::vector<vk::DescriptorSetLayoutBinding> bindings {};
    bindings.emplace_back(vk::DescriptorSetLayoutBinding {
        .binding = 0,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1,
        .stageFlags = vk::ShaderStageFlagBits::eAllGraphics | vk::ShaderStageFlagBits::eCompute,
    });
    std::vector<std::string_view> names { "BloomSettingsUBO" };

    _descriptorSetLayout = PipelineBuilder::CacheDescriptorSetLayout(*_context->GetVulkanContext(), bindings, names);
    vkContext->DebugSetObjectName(_descriptorSetLayout, "Bloom settings DSL");
}

void BloomSettings::CreateUniformBuffers()
{
    auto vkContext { _context->GetVulkanContext() };
    auto resources { _context->Resources() };

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        std::string name = "[] Bloom settings UBO";

        // Inserts i in the middle of []
        name.insert(1, 1, static_cast<char>(i + '0'));

        _frameData.buffers.at(i) = resources->GetBufferResourceManager().Create(
            sizeof(FrameData),
            bb::BufferFlags::UNIFORM_USAGE,
            name.c_str());
    }

    std::array<vk::DescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> layouts {};
    std::for_each(layouts.begin(), layouts.end(), [this](auto& l)
        { l = _descriptorSetLayout; });
    vk::DescriptorSetAllocateInfo allocateInfo {};
    allocateInfo.descriptorPool = vkContext->DescriptorPool();
    allocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
    allocateInfo.pSetLayouts = layouts.data();

    vk::Device device = vkContext->Device();
    util::VK_ASSERT(device.allocateDescriptorSets(&allocateInfo, _frameData.descriptorSets.data()),
        "Failed allocating descriptor sets!");

    for (size_t i = 0; i < _frameData.descriptorSets.size(); ++i)
    {
        UpdateDescriptorSet(i);
    }
}

void BloomSettings::UpdateDescriptorSet(uint32_t currentFrame)
{
    auto vkContext { _context->GetVulkanContext() };
    auto resources { _context->Resources() };

    const bb::Buffer* buffer = resources->GetBufferResourceManager().Access(_frameData.buffers.at(currentFrame));

    vk::DescriptorBufferInfo bufferInfo {};
    bufferInfo.buffer = buffer->buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(SettingsData);

    std::array<vk::WriteDescriptorSet, 1> descriptorWrites {};

    vk::WriteDescriptorSet& bufferWrite { descriptorWrites[0] };
    bufferWrite.dstSet = _frameData.descriptorSets.at(currentFrame);
    bufferWrite.dstBinding = 0;
    bufferWrite.dstArrayElement = 0;
    bufferWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
    bufferWrite.descriptorCount = 1;
    bufferWrite.pBufferInfo = &bufferInfo;

    vk::Device device = vkContext->Device();
    device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}
