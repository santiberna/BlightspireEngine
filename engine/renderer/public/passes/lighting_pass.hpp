#pragma once

#include "frame_graph.hpp"
#include "gbuffers.hpp"
#include "settings.hpp"

#include <memory>

class BloomSettings;
class GraphicsContext;
class CameraResource;
class GPUScene;
struct GPUImage;
struct RenderSceneDescription;

class LightingPass final : public FrameGraphRenderPass
{
public:
    LightingPass(const std::shared_ptr<GraphicsContext>& context, const Settings::Lighting& lightingSettings, const GPUScene& scene, const GBuffers& gBuffers, const ResourceHandle<GPUImage>& hdrTarget, const ResourceHandle<GPUImage>& brightnessTarget, const BloomSettings& bloomSettings, const ResourceHandle<GPUImage>& ssaoTarget);
    ~LightingPass() final;

    void RecordCommands(vk::CommandBuffer commandBuffer, bb::u32 currentFrame, const RenderSceneDescription& scene) final;

    NON_MOVABLE(LightingPass);
    NON_COPYABLE(LightingPass);

private:
    struct PushConstants
    {
        bb::u32 albedoMIndex;
        bb::u32 normalRIndex;
        bb::u32 ssaoIndex;
        bb::u32 depthIndex;
        glm::vec2 screenSize;
        glm::vec2 padding;
        glm::ivec3 clusterDimensions;
        float shadowMapSize;
        float ambientStrength;
        float ambientShadowStrength;
        float decalNormalThreshold;
    } _pushConstants;

    void CreatePipeline();

    std::shared_ptr<GraphicsContext> _context;
    const Settings::Lighting& _lightingSettings;
    const GBuffers& _gBuffers;
    const ResourceHandle<GPUImage> _hdrTarget;
    const ResourceHandle<GPUImage> _brightnessTarget;

    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _pipeline;

    const BloomSettings& _bloomSettings;
};
