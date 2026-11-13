#pragma once

#include "frame_graph.hpp"
#include "resource_manager.hpp"
#include "vulkan/vulkan.hpp"

#include <array>
#include <memory>

class GraphicsContext;

class GPUParticleEffectPass : public FrameGraphRenderPass
{
public:
    GPUParticleEffectPass(const std::shared_ptr<GraphicsContext>& context);
    virtual ~GPUParticleEffectPass();


    void RecordCommands(vk::CommandBuffer commandBuffer, [[maybe_unused]] bb::u32 curentFrame, const RenderSceneDescription& scene) final;

private:
    void BuildInitCommands(vk::CommandBuffer commandBuffer, const RenderSceneDescription& scene);
    void BuildUpdateCommands(vk::CommandBuffer commandBuffer, const RenderSceneDescription& scene);

private:
    void CreateDescriptorSetLayout();
    void AllocateDescriptorSets();
    void CreateBuffers();
    void CreatePipelines();

    struct Particle
    {
        float value;
    };

private:
    static constexpr bb::u8 PARTICLE_BUFFER_COUNT = 2;
    static constexpr bb::u32 PARTICLE_BUFFER_ELEMENT_COUNT = 64;

    std::shared_ptr<GraphicsContext> _context = {};

    vk::PipelineLayout _initPipelineLayout{};
    vk::Pipeline _initPipeline{};

    vk::PipelineLayout _updatePipelineLayout{};
    vk::Pipeline _updatePipeline{};

    vk::PipelineLayout _spawnPipelineLayout{};
    vk::Pipeline _spawnPipeline{};

    // We are using  a single layout for all different shaders
    // Push constants might differ
    // -RWStructuredBuffer<Particle>[2]
    // -RWStructuredBuffer<Atomic<uint32_t>[2]
    vk::DescriptorSetLayout _setLayout = {};

    vk::DescriptorSet _descriptorSet;

    struct ParticleBuffer
    {
        ResourceHandle<bb::Buffer> particleBuffer;
        ResourceHandle<bb::Buffer> atomicCounterBuffer;
    };

    bb::u32 _bufferIndex = 0;
    bb::u32 _backBufferIndex = 0;
    std::array<ParticleBuffer, PARTICLE_BUFFER_COUNT> _particleBuffers;

};