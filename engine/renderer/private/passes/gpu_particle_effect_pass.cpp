#include "passes/gpu_particle_effect_pass.hpp"

#include "colors.hpp"
#include "common.hpp"
#include "gpu_scene.hpp"
#include "graphics_context.hpp"
#include "graphics_resources.hpp"
#include "pipeline_builder.hpp"
#include "resources/buffer.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan_helper.hpp"
#include "shaders/shader_loader.hpp"
#include "gpu_scene.hpp"


GPUParticleEffectPass::GPUParticleEffectPass(const std::shared_ptr<GraphicsContext>& context)
    : _context(context)
{
    CreateBuffers();
    CreateDescriptorSetLayout();
    AllocateDescriptorSets();
    CreatePipelines();
}

GPUParticleEffectPass::~GPUParticleEffectPass()
{
    auto resources = _context->Resources();
    vk::Device device = _context->GetVulkanContext()->Device();

    for(auto& buffer : _particleBuffers)
    {
        resources->GetBufferResourceManager().Destroy(buffer.particleBuffer);
        resources->GetBufferResourceManager().Destroy(buffer.atomicCounterBuffer);
    }
    
    device.destroy(_initPipelineLayout);
    device.destroy(_initPipeline);
    device.destroy(_updatePipelineLayout);
    device.destroy(_updatePipeline);
    device.destroy(_spawnPipelineLayout);
    device.destroy(_spawnPipeline);

    device.destroy(_setLayout);
}

void GPUParticleEffectPass::RecordCommands(vk::CommandBuffer commandBuffer, bb::u32 frame, const RenderSceneDescription& scene)
{
    TracyVkZone(scene.tracyContext, commandBuffer, "GPU Particle Effect Pass");

    vk::Device device = _context->GetVulkanContext()->Device();
    auto resources = _context->Resources();

    _backBufferIndex = _bufferIndex;
    _bufferIndex = ++_bufferIndex % PARTICLE_BUFFER_COUNT;

    BuildInitCommands(commandBuffer, scene);
    BuildUpdateCommands(commandBuffer, scene);
    BuildSpawnCommands(commandBuffer);
}

void GPUParticleEffectPass::BuildInitCommands(vk::CommandBuffer commandBuffer, const RenderSceneDescription& scene)
{
    util::BeginLabel(commandBuffer, "Init", GetColor(ColorType::DarkRed));
    
    struct PushConstants {
        uint32_t backBufferIndex;
        uint32_t bufferIndex;
    } pushConstants {
        .backBufferIndex = _backBufferIndex,
        .bufferIndex = _bufferIndex
    };

    commandBuffer.pushConstants<PushConstants>(_initPipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, pushConstants);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, _initPipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, _initPipelineLayout, 0, _descriptorSet, {});

    commandBuffer.dispatch(1, 1, 1);

    SyncComputeWriteToReadCommand(commandBuffer);

    util::EndLabel(commandBuffer);
}

void GPUParticleEffectPass::BuildUpdateCommands(vk::CommandBuffer commandBuffer, const RenderSceneDescription& scene)
{
    util::BeginLabel(commandBuffer, "Update", GetColor(ColorType::DarkRed));

    struct PushConstants {
        float deltaTime;
        uint32_t backBufferIndex;
        uint32_t bufferIndex;
    } pushConstants {
        .deltaTime = scene.deltaTime,
        .backBufferIndex = _backBufferIndex,
        .bufferIndex = _bufferIndex
    };

    commandBuffer.pushConstants<PushConstants>(_updatePipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, pushConstants);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, _updatePipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, _updatePipelineLayout, 0, _descriptorSet, {});

    commandBuffer.dispatch(1, 1, 1);

    SyncComputeWriteToReadCommand(commandBuffer);

    util::EndLabel(commandBuffer);
}

void GPUParticleEffectPass::BuildSpawnCommands(vk::CommandBuffer commandBuffer)
{
    util::BeginLabel(commandBuffer, "Spawn", GetColor(ColorType::DarkRed));

    struct PushConstants {
        uint32_t backBufferIndex;
        uint32_t bufferIndex;
    } pushConstants {
        .backBufferIndex = _backBufferIndex,
        .bufferIndex = _bufferIndex
    };

    commandBuffer.pushConstants<PushConstants>(_spawnPipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, pushConstants);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, _spawnPipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, _spawnPipelineLayout, 0, _descriptorSet, {});

    commandBuffer.dispatch(1, 1, 1);

    util::EndLabel(commandBuffer);
}

void GPUParticleEffectPass::SyncComputeWriteToReadCommand(vk::CommandBuffer commandBuffer)
{
    vk::MemoryBarrier2 memoryBarrier = {};
    memoryBarrier.srcStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    memoryBarrier.srcAccessMask = vk::AccessFlagBits2::eShaderStorageWrite;
    memoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    memoryBarrier.dstAccessMask = vk::AccessFlagBits2::eShaderStorageRead;

    vk::DependencyInfo dependencyInfo = {
        .memoryBarrierCount = 1,
        .pMemoryBarriers = &memoryBarrier
    };

    commandBuffer.pipelineBarrier2(dependencyInfo);
}


void GPUParticleEffectPass::CreateBuffers()
{
    std::shared_ptr<GraphicsResources> resources = _context->Resources();

    for (bb::u8 i = 0; i < PARTICLE_BUFFER_COUNT; i++)
    {
        bb::Flags<bb::BufferFlags> flags = { bb::BufferFlags::STORAGE_USAGE };

        _particleBuffers[i].particleBuffer = resources->GetBufferResourceManager().Create(
            sizeof(Particle) * PARTICLE_BUFFER_ELEMENT_COUNT,
            flags,
            std::string("GPU Particle effect buffer ").append(std::to_string(i)).c_str());

        _particleBuffers[i].atomicCounterBuffer = resources->GetBufferResourceManager().Create(
            sizeof(bb::u32),
            flags,
            std::string("GPU Particle effect buffer atomic counter").append(std::to_string(i)).c_str()
        );
    }
}

void GPUParticleEffectPass::CreateDescriptorSetLayout()
{
    vk::Device device = _context->GetVulkanContext()->Device();

    vk::DescriptorSetLayoutBinding layoutBindings[2] = {};
    for(bb::u32 i = 0; i < 2; i++)
    {
        layoutBindings[i].binding = i;
        layoutBindings[i].descriptorCount = 2;
        layoutBindings[i].descriptorType = vk::DescriptorType::eStorageBuffer;
        layoutBindings[i].stageFlags = vk::ShaderStageFlagBits::eCompute;
    }


    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = layoutBindings;

    _setLayout = device.createDescriptorSetLayout(layoutInfo).value;
}

void GPUParticleEffectPass::AllocateDescriptorSets()
{
    auto resources = _context->Resources();
    std::shared_ptr<VulkanContext> vkContext = _context->GetVulkanContext();
    vk::Device device = _context->GetVulkanContext()->Device();

    vk::DescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.descriptorPool = vkContext->DescriptorPool();
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &_setLayout;
    util::VK_ASSERT(device.allocateDescriptorSets(&allocateInfo, &_descriptorSet), 
    "Failed to allocate gpu particle effect descriptor set");

    vk::DescriptorBufferInfo particleBufferInfos[2] = {};
    vk::DescriptorBufferInfo counterBufferInfos[2] = {};

    for(bb::u8 i = 0; i < 2; i++)
    {
        particleBufferInfos[i].buffer = resources->GetBufferResourceManager().Access(_particleBuffers[i].particleBuffer)->buffer;
        particleBufferInfos[i].offset = 0;
        particleBufferInfos[i].range = vk::WholeSize;
        
        counterBufferInfos[i].buffer = resources->GetBufferResourceManager().Access(_particleBuffers[i].atomicCounterBuffer)->buffer;
        counterBufferInfos[i].offset = 0;
        counterBufferInfos[i].range = vk::WholeSize;
    }

    vk::WriteDescriptorSet writeSets[2] = {};
    writeSets[0].dstSet = _descriptorSet;
    writeSets[0].dstBinding = 0;
    writeSets[0].descriptorType = vk::DescriptorType::eStorageBuffer;
    writeSets[0].descriptorCount = 2;
    writeSets[0].pBufferInfo = particleBufferInfos;

    writeSets[1].dstSet = _descriptorSet;
    writeSets[1].dstBinding = 1;
    writeSets[1].descriptorType = vk::DescriptorType::eStorageBuffer;
    writeSets[1].descriptorCount = 2;
    writeSets[1].pBufferInfo = counterBufferInfos;

    device.updateDescriptorSets(2, writeSets, 0, nullptr);
}


void GPUParticleEffectPass::CreatePipelines()
{
    vk::Device device = _context->GetVulkanContext()->Device();

    vk::PipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &_setLayout;
    
    // Init pipeline
    {
        vk::PushConstantRange pcRange{};
        pcRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
        pcRange.offset = 0;
        pcRange.size = 2 * sizeof(int32_t);

        layoutInfo.pushConstantRangeCount = 1;
        layoutInfo.pPushConstantRanges = &pcRange;

        _initPipelineLayout = device.createPipelineLayout(layoutInfo).value;

        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges = nullptr;

        std::vector<std::byte> shader = shader::ReadFile("slang_shaders/bin/particles/particle_init.spv");

        vk::ShaderModuleCreateInfo moduleCi{
            .codeSize = shader.size(),
            .pCode = reinterpret_cast<const bb::u32*>(shader.data())
        };

        vk::ShaderModule shaderModule = device.createShaderModule(moduleCi).value;

        vk::PipelineShaderStageCreateInfo stageCi{
            .stage = vk::ShaderStageFlagBits::eCompute,
            .module = shaderModule,
            .pName = "main"
        };

        vk::ComputePipelineCreateInfo pipelineCi{
            .stage = stageCi,
            .layout = _initPipelineLayout
        };

        _initPipeline = device.createComputePipeline(VK_NULL_HANDLE, pipelineCi).value;

        device.destroy(shaderModule);
    }
    // Update pipeline
    {
        vk::PushConstantRange pcRange{};
        pcRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
        pcRange.offset = 0;
        pcRange.size = 2 * sizeof(int32_t) + sizeof(float);

        layoutInfo.pushConstantRangeCount = 1;
        layoutInfo.pPushConstantRanges = &pcRange;

        _updatePipelineLayout = device.createPipelineLayout(layoutInfo).value;

        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges = nullptr;

        std::vector<std::byte> shader = shader::ReadFile("slang_shaders/bin/particles/particle_update.spv");

        vk::ShaderModuleCreateInfo moduleCi{
            .codeSize = shader.size(),
            .pCode = reinterpret_cast<const bb::u32*>(shader.data())
        };

        vk::ShaderModule shaderModule = device.createShaderModule(moduleCi).value;

        vk::PipelineShaderStageCreateInfo stageCi{
            .stage = vk::ShaderStageFlagBits::eCompute,
            .module = shaderModule,
            .pName = "main"
        };

        vk::ComputePipelineCreateInfo pipelineCi{
            .stage = stageCi,
            .layout = _updatePipelineLayout
        };

        _updatePipeline = device.createComputePipeline(VK_NULL_HANDLE, pipelineCi).value;

        device.destroy(shaderModule);
    }

      // Spawn pipeline
    {
        vk::PushConstantRange pcRange{};
        pcRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
        pcRange.offset = 0;
        pcRange.size = 2 * sizeof(int32_t);

        layoutInfo.pushConstantRangeCount = 1;
        layoutInfo.pPushConstantRanges = &pcRange;

        _spawnPipelineLayout = device.createPipelineLayout(layoutInfo).value;

        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges = nullptr;

        std::vector<std::byte> shader = shader::ReadFile("slang_shaders/bin/particles/particle_spawn.spv");

        vk::ShaderModuleCreateInfo moduleCi{
            .codeSize = shader.size(),
            .pCode = reinterpret_cast<const bb::u32*>(shader.data())
        };

        vk::ShaderModule shaderModule = device.createShaderModule(moduleCi).value;

        vk::PipelineShaderStageCreateInfo stageCi{
            .stage = vk::ShaderStageFlagBits::eCompute,
            .module = shaderModule,
            .pName = "main"
        };

        vk::ComputePipelineCreateInfo pipelineCi{
            .stage = stageCi,
            .layout = _spawnPipelineLayout
        };

        _spawnPipeline = device.createComputePipeline(VK_NULL_HANDLE, pipelineCi).value;

        device.destroy(shaderModule);
    }
}