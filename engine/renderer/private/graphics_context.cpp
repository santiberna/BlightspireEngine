#include "graphics_context.hpp"

#include "graphics_resources.hpp"
#include "pipeline_builder.hpp"
#include "resource_management/buffer_resource_manager.hpp"
#include "resource_management/image_resource_manager.hpp"
#include "resource_management/material_resource_manager.hpp"
#include "resource_management/sampler_resource_manager.hpp"
#include "vulkan_context.hpp"
#include "vulkan_helper.hpp"

#include <tracy/Tracy.hpp>

struct GraphicsContext::BindlessObjects
{
    // BINDLESS THINGS
    vk::DescriptorPool _bindlessPool;
    vk::DescriptorSetLayout _bindlessLayout;
    vk::DescriptorSet _bindlessSet;

    std::array<vk::DescriptorImageInfo, MAX_BINDLESS_RESOURCES> _bindlessImageInfos;
    std::array<vk::WriteDescriptorSet, MAX_BINDLESS_RESOURCES> _bindlessImageWrites;

    ResourceHandle<Buffer> _bindlessMaterialBuffer;
    vk::DescriptorBufferInfo _bindlessMaterialInfo;
    vk::WriteDescriptorSet _bindlessMaterialWrite;
};

GraphicsContext::GraphicsContext(SDL_Window* window)
    : _drawStats()
{
    ZoneScopedN("Graphics Backend Initialization");

    bindless = std::make_unique<BindlessObjects>();
    _vulkanContext = std::make_shared<VulkanContext>(window);
    _graphicsResources = std::make_shared<GraphicsResources>(_vulkanContext);

    CreateBindlessMaterialBuffer();
    CreateBindlessDescriptorSet();

    bb::Image2D fallback {};
    fallback.data = std::make_shared<std::byte[]>(2 * 2 * 4);
    fallback.height = 2;
    fallback.width = 2;
    fallback.format = bb::ImageFormat::R8G8B8A8_UNORM;

    _fallbackImage = _graphicsResources->GetImageResourceManager().Create(fallback, bb::TextureFlags::COMMON_FLAGS, "Fallback Texture");
}

GraphicsContext::~GraphicsContext()
{
    vk::Device device = _vulkanContext->Device();
    device.destroy(bindless->_bindlessLayout);
    device.destroy(bindless->_bindlessPool);
}

VkDescriptorSetLayout GraphicsContext::BindlessLayout() const { return bindless->_bindlessLayout; }
VkDescriptorSet GraphicsContext::BindlessSet() const { return bindless->_bindlessSet; }

void GraphicsContext::CreateBindlessDescriptorSet()
{
    vk::Device device = _vulkanContext->Device();

    std::array<vk::DescriptorPoolSize, 6> poolSizes = {
        vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, MAX_BINDLESS_RESOURCES },
        vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, MAX_BINDLESS_RESOURCES },
        vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, MAX_BINDLESS_RESOURCES },
        vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, MAX_BINDLESS_RESOURCES },
        vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, MAX_BINDLESS_RESOURCES },
        vk::DescriptorPoolSize { vk::DescriptorType::eStorageBuffer, 1 },
    };

    vk::DescriptorPoolCreateInfo poolCreateInfo {};
    poolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
    poolCreateInfo.maxSets = MAX_BINDLESS_RESOURCES * poolSizes.size();
    poolCreateInfo.poolSizeCount = poolSizes.size();
    poolCreateInfo.pPoolSizes = poolSizes.data();

    util::VK_ASSERT(device.createDescriptorPool(&poolCreateInfo, nullptr, &bindless->_bindlessPool), "Failed creating bindless pool!");

    std::vector<vk::DescriptorSetLayoutBinding> bindings(3);
    vk::DescriptorSetLayoutBinding& combinedImageSampler = bindings[0];
    combinedImageSampler.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    combinedImageSampler.descriptorCount = MAX_BINDLESS_RESOURCES;
    combinedImageSampler.binding = static_cast<uint32_t>(BindlessBinding::eImage);
    combinedImageSampler.stageFlags = vk::ShaderStageFlagBits::eAllGraphics | vk::ShaderStageFlagBits::eCompute;

    vk::DescriptorSetLayoutBinding& storageImage = bindings[1];
    storageImage.descriptorType = vk::DescriptorType::eStorageImage;
    storageImage.descriptorCount = MAX_BINDLESS_RESOURCES;
    storageImage.binding = static_cast<uint32_t>(BindlessBinding::eStorageImage);
    storageImage.stageFlags = vk::ShaderStageFlagBits::eAllGraphics | vk::ShaderStageFlagBits::eCompute;

    vk::DescriptorSetLayoutBinding& materialBinding = bindings[2];
    materialBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    materialBinding.descriptorCount = 1;
    materialBinding.binding = static_cast<uint32_t>(BindlessBinding::eStorageBuffer);
    materialBinding.stageFlags = vk::ShaderStageFlagBits::eAllGraphics | vk::ShaderStageFlagBits::eCompute;

    vk::StructureChain<vk::DescriptorSetLayoutCreateInfo, vk::DescriptorSetLayoutBindingFlagsCreateInfo> structureChain;

    auto& layoutCreateInfo = structureChain.get<vk::DescriptorSetLayoutCreateInfo>();
    layoutCreateInfo.bindingCount = bindings.size();
    layoutCreateInfo.pBindings = bindings.data();
    layoutCreateInfo.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;

    std::array<vk::DescriptorBindingFlagsEXT, 3> bindingFlags = {
        vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateAfterBind,
        vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateAfterBind,
        vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateAfterBind,
    };

    auto& extInfo = structureChain.get<vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT>();
    extInfo.bindingCount = bindings.size();
    extInfo.pBindingFlags = bindingFlags.data();

    std::vector<std::string_view> names { "bindless_color_textures", "bindless_storage_image_r16f", "Materials" };

    bindless->_bindlessLayout = PipelineBuilder::CacheDescriptorSetLayout(*_vulkanContext, bindings, names, layoutCreateInfo);

    vk::DescriptorSetAllocateInfo allocInfo {};
    allocInfo.descriptorPool = bindless->_bindlessPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &bindless->_bindlessLayout;

    util::VK_ASSERT(device.allocateDescriptorSets(&allocInfo, &bindless->_bindlessSet), "Failed creating bindless descriptor set!");
    _vulkanContext->DebugSetObjectName(bindless->_bindlessSet, "Bindless DS");
}

void GraphicsContext::CreateBindlessMaterialBuffer()
{
    BufferCreation creation {};
    creation.SetSize(MAX_BINDLESS_RESOURCES * sizeof(GPUMaterial::GPUInfo))
        .SetUsageFlags(vk::BufferUsageFlagBits::eUniformBuffer)
        .SetName("Bindless material uniform buffer");

    bindless->_bindlessMaterialBuffer = _graphicsResources->GetBufferResourceManager().Create(creation);
}

void GraphicsContext::UpdateBindlessSet()
{
    UpdateBindlessImages();
    UpdateBindlessMaterials();
}

void GraphicsContext::UpdateBindlessImages()
{
    ImageResourceManager& imageResourceManager { _graphicsResources->GetImageResourceManager() };
    SamplerResourceManager& samplers { _graphicsResources->GetSamplerResourceManager() };

    // Default sampler
    if (!_sampler.isValid())
    {
        bb::SamplerCreation createInfo {
            .name = "Graphics context sampler",
            .maxLod = vk::LodClampNone,
        };

        _sampler = samplers.Create(createInfo);
    }

    for (uint32_t i = 0; i < MAX_BINDLESS_RESOURCES; ++i)
    {
        auto* fallback = imageResourceManager.Access(_fallbackImage);
        bindless->_bindlessImageInfos.at(i).imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        bindless->_bindlessImageInfos.at(i).imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        bindless->_bindlessImageInfos.at(i).imageView = fallback->view;
        bindless->_bindlessImageInfos.at(i).sampler = samplers.Access(_sampler)->sampler;
    }

    for (auto [handle, texture] : imageResourceManager.Resources())
    {
        auto i = handle.getIndex();
        assert(i < MAX_BINDLESS_RESOURCES);

        if (!(texture.flags & vk::ImageUsageFlagBits::eSampled))
        {
            continue;
        }

        bindless->_bindlessImageInfos.at(i).imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        bindless->_bindlessImageInfos.at(i).imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        bindless->_bindlessImageInfos.at(i).imageView = texture.view;
        bindless->_bindlessImageInfos.at(i).sampler = samplers.Access(texture.sampler)->sampler;
    }

    for (uint32_t i = 0; i < MAX_BINDLESS_RESOURCES; ++i)
    {
        bindless->_bindlessImageWrites.at(i).dstSet = bindless->_bindlessSet;
        bindless->_bindlessImageWrites.at(i).dstBinding = static_cast<uint32_t>(BindlessBinding::eImage);
        bindless->_bindlessImageWrites.at(i).dstArrayElement = i;
        bindless->_bindlessImageWrites.at(i).descriptorType = vk::DescriptorType::eCombinedImageSampler;
        bindless->_bindlessImageWrites.at(i).descriptorCount = 1;
        bindless->_bindlessImageWrites.at(i).pImageInfo = &bindless->_bindlessImageInfos[i];
    }

    vk::Device device = _vulkanContext->Device();
    device.updateDescriptorSets(MAX_BINDLESS_RESOURCES, bindless->_bindlessImageWrites.data(), 0, nullptr);
}

void GraphicsContext::UpdateBindlessMaterials()
{
    MaterialResourceManager& materialResourceManager { _graphicsResources->GetMaterialResourceManager() };
    BufferResourceManager& bufferResourceManager { _graphicsResources->GetBufferResourceManager() };

    if (materialResourceManager.Resources().storageSize() == 0)
    {
        return;
    }

    assert(materialResourceManager.Resources().storageSize() < MAX_BINDLESS_RESOURCES && "There are more materials used than the amount that can be stored on the GPU.");

    std::array<GPUMaterial::GPUInfo, MAX_BINDLESS_RESOURCES> materialGPUData;
    for (auto [handle, material] : materialResourceManager.Resources())
    {
        materialGPUData[handle.getIndex()] = material.gpuInfo;
    }

    const Buffer* buffer = bufferResourceManager.Access(bindless->_bindlessMaterialBuffer);
    std::memcpy(buffer->mappedPtr, materialGPUData.data(), materialResourceManager.Resources().storageSize() * sizeof(GPUMaterial::GPUInfo));

    bindless->_bindlessMaterialInfo.buffer = buffer->buffer;
    bindless->_bindlessMaterialInfo.offset = 0;
    bindless->_bindlessMaterialInfo.range = sizeof(GPUMaterial::GPUInfo) * materialResourceManager.Resources().storageSize();

    bindless->_bindlessMaterialWrite.dstSet = bindless->_bindlessSet;
    bindless->_bindlessMaterialWrite.dstBinding = static_cast<uint32_t>(BindlessBinding::eStorageBuffer);
    bindless->_bindlessMaterialWrite.dstArrayElement = 0;
    bindless->_bindlessMaterialWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
    bindless->_bindlessMaterialWrite.descriptorCount = 1;
    bindless->_bindlessMaterialWrite.pBufferInfo = &bindless->_bindlessMaterialInfo;

    vk::Device device = _vulkanContext->Device();
    device.updateDescriptorSets(1, &bindless->_bindlessMaterialWrite, 0, nullptr);
}
