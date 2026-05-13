#pragma once

#include "frame_graph.hpp"
#include "settings.hpp"

class DOFPass : public FrameGraphRenderPass
{
public:
    DOFPass(const std::shared_ptr<GraphicsContext>& context);
    ~DOFPass() override;

    virtual void RecordCommands(vk::CommandBuffer commandBuffer, bb::u32 currentFrame, const RenderSceneDescription& scene) override;

    NON_MOVABLE(DOFPass);
    NON_COPYABLE(DOFPass);

private:
    void CreatePipeline();

    std::shared_ptr<GraphicsContext> _context;

    const ResourceHandle<GPUImage> _dofTarget;
    const ResourceHandle<GPUImage> _source;

    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _pipeline;
};
