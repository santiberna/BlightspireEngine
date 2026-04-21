#pragma once

#include "frame_graph.hpp"

class SwapChain;
class GraphicsContext;
struct RenderSceneDescription;
class GPUScene;

class ClusterLightCullingPass final : public FrameGraphRenderPass
{
public:
    ClusterLightCullingPass(const std::shared_ptr<GraphicsContext>& context, GPUScene& gpuScene,
        ResourceHandle<bb::Buffer>& clusterBuffer, ResourceHandle<bb::Buffer>& globalIndex,
        ResourceHandle<bb::Buffer>& lightCells, ResourceHandle<bb::Buffer>& lightIndices);
    ~ClusterLightCullingPass() final;

    void RecordCommands(vk::CommandBuffer commandBuffer, uint32_t currentFrame, const RenderSceneDescription& scene) final;

    NON_MOVABLE(ClusterLightCullingPass);
    NON_COPYABLE(ClusterLightCullingPass);

private:
    void CreatePipeline();

    std::shared_ptr<GraphicsContext> _context;
    GPUScene& _gpuScene;

    ResourceHandle<bb::Buffer> _clusterBuffer;
    ResourceHandle<bb::Buffer> _globalIndex;
    ResourceHandle<bb::Buffer> _lightCells;
    ResourceHandle<bb::Buffer> _lightIndices;

    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _pipeline;
};