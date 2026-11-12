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


    void RecordCommands(vk::CommandBuffer commandBuffer, [[maybe_unused]] bb::u32 curentFrame, const RenderSceneDescription& Scene) final;

private:
    void CreateBuffers();
    void CreatePipelines();
    void AllocateDescriptorSets();

    struct Particle
    {
        bb::u32 value;
    };

private:
    static constexpr bb::u8 PARTICLE_BUFFER_COUNT = 2;
    static constexpr bb::u32 PARTICLE_BUFFER_ELEMENT_COUNT = 64;

    std::shared_ptr<GraphicsContext> _context = {};

    vk::PipelineLayout _pipelineLayout{};
    vk::Pipeline _pipeline{};
    vk::DescriptorSetLayout _setLayout = {};

    std::array<vk::DescriptorSet, PARTICLE_BUFFER_COUNT> _descriptorSets = {};

    bb::u8 _index = 0;
    std::array<ResourceHandle<bb::Buffer>, PARTICLE_BUFFER_COUNT> _particleBuffers;

};