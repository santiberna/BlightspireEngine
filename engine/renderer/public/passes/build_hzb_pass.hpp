#pragma once

#include "frame_graph.hpp"

class CameraBatch;

class BuildHzbPass final : public FrameGraphRenderPass
{
public:
    BuildHzbPass(const std::shared_ptr<GraphicsContext>& context, CameraBatch& cameraBatch, vk::DescriptorSetLayout hzbImageDSL);
    ~BuildHzbPass() final;
    void RecordCommands(vk::CommandBuffer commandBuffer, bb::u32 currentFrame, const RenderSceneDescription& scene) final;

private:
    std::shared_ptr<GraphicsContext> _context;
    CameraBatch& _cameraBatch;

    vk::PipelineLayout _buildHzbPipelineLayout;
    vk::Pipeline _buildHzbPipeline;
    vk::DescriptorSetLayout _hzbImageDSL;
    vk::DescriptorUpdateTemplate _hzbUpdateTemplate;

    ResourceHandle<bb::Sampler> _hzbSampler;

    void CreateSampler();
    void CreatPipeline();
    void CreateUpdateTemplate();
};
