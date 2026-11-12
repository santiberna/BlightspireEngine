#include "passes/gpu_particle_effect_pass.hpp"

#include "colors.hpp"
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
    CreatePipelines();
    AllocateDescriptorSets();
}

GPUParticleEffectPass::~GPUParticleEffectPass()
{
    auto resources = _context->Resources();
    vk::Device device = _context->GetVulkanContext()->Device();

    device.destroy(_pipelineLayout);
    device.destroy(_pipeline);
    for(auto& buffer : _particleBuffers)
    {
        resources->GetBufferResourceManager().Destroy(buffer);
    }

    device.destroy(_setLayout);
}

void GPUParticleEffectPass::RecordCommands(vk::CommandBuffer commandBuffer, bb::u32 frame, const RenderSceneDescription& scene)
{
    TracyVkZone(scene.tracyContext, commandBuffer, "GPU Particle Effect Pass");

    vk::Device device = _context->GetVulkanContext()->Device();
    auto resources = _context->Resources();

    _index = _index % PARTICLE_BUFFER_COUNT;

    util::BeginLabel(commandBuffer, "Populating particle buffer", GetColor(ColorType::DarkRed));

    struct PushConstants {
        float deltaTime;
    } pushConstants {
        .deltaTime = scene.deltaTime
    };

    commandBuffer.pushConstants<PushConstants>(_pipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, pushConstants);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, _pipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, _pipelineLayout, 0, { _descriptorSets[_index] }, {});

    commandBuffer.dispatch(1, 1, 1);

    util::EndLabel(commandBuffer);
}

void GPUParticleEffectPass::CreateBuffers()
{
    std::shared_ptr<GraphicsResources> resources = _context->Resources();

    for (bb::u8 i = 0; i < PARTICLE_BUFFER_COUNT; i++)
    {
        bb::Flags<bb::BufferFlags> flags = { bb::BufferFlags::STORAGE_USAGE };
        _particleBuffers[i] = resources->GetBufferResourceManager().Create(
            sizeof(Particle) * PARTICLE_BUFFER_ELEMENT_COUNT,
            flags,
            std::string("GPU Particle effect buffer ").append(std::to_string(i)).c_str());
    }
}

void GPUParticleEffectPass::CreatePipelines()
{
    ComputePipelineBuilder builder(_context);

    std::vector<std::byte> shader = shader::ReadFile("slang_shaders/bin/particles/depthbuffer_sampled_particles.spv");
    builder.AddShaderStage(vk::ShaderStageFlagBits::eCompute, shader, "main");

    auto result  = builder.BuildPipeline();
    _pipelineLayout = std::get<0>(result);
    _pipeline = std::get<1>(result);
    _setLayout = builder.GetDescriptorSetLayouts()[0];
}

void GPUParticleEffectPass::AllocateDescriptorSets()
{
    auto resources = _context->Resources();
    std::shared_ptr<VulkanContext> vkContext = _context->GetVulkanContext();
    vk::Device device = _context->GetVulkanContext()->Device();

    std::array<vk::DescriptorSetLayout, PARTICLE_BUFFER_COUNT> setLayouts{};
    setLayouts.fill(_setLayout);

    vk::DescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.descriptorPool = vkContext->DescriptorPool();
    allocateInfo.descriptorSetCount = PARTICLE_BUFFER_COUNT;
    allocateInfo.pSetLayouts = setLayouts.data();
    util::VK_ASSERT(device.allocateDescriptorSets(&allocateInfo, _descriptorSets.data()), 
    "Failed to allocate gpu particle effect descriptor set");

    
    for(bb::u32 i = 0; i < PARTICLE_BUFFER_COUNT; i++)
    {
        vk::DescriptorBufferInfo particleBufferInfo{};
        particleBufferInfo.buffer = resources->GetBufferResourceManager().Access(_particleBuffers[i])->buffer;
        particleBufferInfo.offset = 0;
        particleBufferInfo.range = resources->GetBufferResourceManager().Access(_particleBuffers[i])->size;

        vk::WriteDescriptorSet writeSet{};
        writeSet.dstSet = _descriptorSets[i];
        writeSet.dstBinding = 0;
        writeSet.descriptorType = vk::DescriptorType::eStorageBuffer;
        writeSet.descriptorCount = 1;
        writeSet.pBufferInfo = &particleBufferInfo;
        
        device.updateDescriptorSets(1, &writeSet, 0, nullptr);
    }
}