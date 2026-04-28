#pragma once

#include "frame_graph.hpp"
#include "gbuffers.hpp"
#include "settings.hpp"

#include <memory>

class BloomSettings;
class GraphicsContext;
class CameraResource;
struct GPUImage;
struct RenderSceneDescription;

class SSAOPass final : public FrameGraphRenderPass
{
public:
    SSAOPass(const std::shared_ptr<GraphicsContext>& context, const Settings::SSAO& settings, const GBuffers& gBuffers, const ResourceHandle<GPUImage>& ssaoTarget);
    ~SSAOPass() final;

    void RecordCommands(vk::CommandBuffer commandBuffer, bb::u32 currentFrame, const RenderSceneDescription& scene) final;

    NON_MOVABLE(SSAOPass);
    NON_COPYABLE(SSAOPass);

private:
    struct PushConstants
    {
        bb::u32 normalIndex;
        bb::u32 depthIndex;
        bb::u32 ssaoNoiseIndex;
        bb::u32 ssaoRenderTargetWidth = 1920 / 2; // just for refference
        bb::u32 ssaoRenderTargetHeight = 1080 / 2;
        float aoStrength = 2.0f;
        float aoBias = 0.01f;
        float aoRadius = 0.2f;
        float minAoDistance = 1.0f;
        float maxAoDistance = 3.0f;
    } _pushConstants;

    void CreatePipeline();
    void CreateBuffers();

    void CreateDescriptorSetLayouts();
    void CreateDescriptorSets();

    ResourceHandle<bb::Buffer> _sampleKernelBuffer;
    ResourceHandle<bb::Sampler> _noiseSampler;

    vk::DescriptorSetLayout _descriptorSetLayout;
    vk::DescriptorSet _descriptorSet;

    std::shared_ptr<GraphicsContext> _context;
    const Settings::SSAO& _settings;
    const GBuffers& _gBuffers;
    const ResourceHandle<GPUImage> _ssaoTarget;

    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _pipeline;

    ResourceHandle<GPUImage> _ssaoNoise;
};
