#pragma once

#include "camera_batch.hpp"
#include "common.hpp"
#include "frame_graph.hpp"

#include <cstdint>
#include <memory>

class GPUScene;
class GraphicsContext;
struct RenderSceneDescription;

class GenerateDrawsPass final : public FrameGraphRenderPass
{
public:
    GenerateDrawsPass(const std::shared_ptr<GraphicsContext>& context, const CameraBatch& cameraBatch, const bool drawStatic, const bool drawDynamic);
    ~GenerateDrawsPass() final;

    void RecordCommands(vk::CommandBuffer commandBuffer, bb::u32 currentFrame, const RenderSceneDescription& scene) final;

    void SetDrawStatic(bool drawStatic) { _shouldDrawStatic = drawStatic; }
    void SetDrawDynamic(bool drawDynamic) { _shouldDrawDynamic = drawDynamic; }

    NON_COPYABLE(GenerateDrawsPass);
    NON_MOVABLE(GenerateDrawsPass);

private:
    std::shared_ptr<GraphicsContext> _context;

    vk::PipelineLayout _generateDrawsPipelineLayout;
    vk::Pipeline _generateDrawsPipeline;

    bool _isPrepass = true;
    bool _shouldDrawStatic = true;
    bool _shouldDrawDynamic = true;
    const bb::u32 _localComputeSize = 64;

    const CameraBatch& _cameraBatch;

    struct PushConstants
    {
        bb::u32 isPrepass;
        float mipSize;
        bb::u32 hzbIndex;
        bb::u32 drawCommandsCount;
        bb::u32 isReverseZ;
        bb::u32 drawStaticDraws;
    };

    void CreateCullingPipeline();
    void RecordPrepassCommands(vk::CommandBuffer commandBuffer, bb::u32 currentFrame, const CameraBatch::Draw& draw, vk::DescriptorSet sceneDraws, vk::DescriptorSet sceneInstances, bb::u32 drawCount, const PushConstants& pc);
    void RecordSecondPassCommands(vk::CommandBuffer commandBuffer, bb::u32 currentFrame, const CameraBatch::Draw& draw, vk::DescriptorSet sceneDraws, vk::DescriptorSet sceneInstances, bb::u32 drawCount, const PushConstants& pc);
};
