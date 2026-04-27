#pragma once

#include "common.hpp"
#include "frame_graph.hpp"
#include "gbuffers.hpp"
#include "resource_manager.hpp"

#include "vulkan_include.hpp"
#include <cstdint>
#include <memory>
#include <vma_include.hpp>

class CameraResource;
class BloomSettings;
class ECSModule;
class GraphicsContext;

struct Emitter;
struct LocalEmitter;
struct RenderSceneDescription;

namespace bb
{
struct Buffer;
}

class ParticlePass final : public FrameGraphRenderPass
{
public:
    ParticlePass(const std::shared_ptr<GraphicsContext>& context, ECSModule& ecs, const GBuffers& gBuffers, const ResourceHandle<GPUImage>& hdrTarget, const ResourceHandle<GPUImage>& brightnessTarget, const BloomSettings& bloomSettings);
    ~ParticlePass() final;

    void RecordCommands(vk::CommandBuffer commandBuffer, bb::u32 currentFrame, const RenderSceneDescription& scene) final;

    void ResetParticles();

    NON_COPYABLE(ParticlePass);
    NON_MOVABLE(ParticlePass);

private:
    enum class ParticleBufferUsage
    {
        eParticle = 0,
        eAliveNew,
        eAliveCurrent,
        eDead,
        eCounter,
        eNone
    };
    enum class ShaderStages
    {
        eKickOff = 0,
        eEmit,
        eSimulate,
        eRenderIndexedIndirect,
        eNone
    };

    struct SimulatePushConstant
    {
        float deltaTime;
        bb::u32 localEmitterCount;
    } _simulatePushConstant;
    struct EmitPushConstant
    {
        bb::u32 bufferOffset;
    } _emitPushConstant;

    std::shared_ptr<GraphicsContext> _context;
    ECSModule& _ecs;
    const GBuffers& _gBuffers;
    const ResourceHandle<GPUImage> _hdrTarget;
    const ResourceHandle<GPUImage> _brightnessTarget;
    const BloomSettings& _bloomSettings;

    std::vector<Emitter> _emitters;
    std::vector<LocalEmitter> _localEmitters;

    std::array<vk::Pipeline, 4> _pipelines;
    std::array<vk::PipelineLayout, 4> _pipelineLayouts;

    // indirect draw storage buffer
    ResourceHandle<bb::Buffer> _drawCommandsBuffer;
    vk::DescriptorSet _drawCommandsDescriptorSet;
    vk::DescriptorSetLayout _drawCommandsDescriptorSetLayout;
    // particle instances storage buffer
    ResourceHandle<bb::Buffer> _culledInstancesBuffer;
    vk::DescriptorSet _culledInstancesDescriptorSet;
    vk::DescriptorSetLayout _culledInstancesDescriptorSetLayout;
    // particle storage buffers
    std::array<ResourceHandle<bb::Buffer>, 5> _particlesBuffers;
    vk::DescriptorSet _particlesBuffersDescriptorSet;
    vk::DescriptorSetLayout _particlesBuffersDescriptorSetLayout;
    // emitter uniform buffers
    ResourceHandle<bb::Buffer> _emittersBuffer;
    vk::DescriptorSet _emittersDescriptorSet;
    vk::DescriptorSetLayout _emittersBufferDescriptorSetLayout;
    // emitter staging buffer
    std::array<vk::Buffer, MAX_FRAMES_IN_FLIGHT> _emitterStagingBuffer;
    std::array<VmaAllocation, MAX_FRAMES_IN_FLIGHT> _emitterStagingBufferAllocation;
    // local emitter uniform buffer
    ResourceHandle<bb::Buffer> _localEmittersBuffer;
    vk::DescriptorSet _localEmittersDescriptorSet;
    vk::DescriptorSetLayout _localEmittersDescriptorSetLayout;
    // local emitter staging buffer
    std::array<vk::Buffer, MAX_FRAMES_IN_FLIGHT> _localEmitterStagingBuffer;
    std::array<VmaAllocation, MAX_FRAMES_IN_FLIGHT> _localEmitterStagingBufferAllocation;
    // buffers for rendering
    ResourceHandle<bb::Buffer> _vertexBuffer;
    ResourceHandle<bb::Buffer> _indexBuffer;

    void RecordKickOff(vk::CommandBuffer commandBuffer);
    void RecordEmit(vk::CommandBuffer commandBuffer);
    void RecordSimulate(vk::CommandBuffer commandBuffer, const CameraResource& camera, float deltaTime, bb::u32 currentFrame);
    void RecordRenderIndexedIndirect(vk::CommandBuffer commandBuffer, const RenderSceneDescription& scene, bb::u32 currentFrame);

    void UpdateEmitters(vk::CommandBuffer commandBuffer, bb::u32 currentFrame);

    void CreatePipelines();
    void CreateDescriptorSetLayouts();
    void CreateDescriptorSets();
    void CreateBuffers();

    void UpdateAliveLists();
    void UpdateParticleBuffersDescriptorSets();
    void UpdateParticleInstancesBufferDescriptorSet();
    void UpdateEmittersBuffersDescriptorSets();
    void UpdateLocalEmittersBuffersDescriptorSets();
    void UpdateDrawCommandsBufferDescriptorSet();
};