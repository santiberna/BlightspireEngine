#include "passes/dof_pass.hpp"

#include "gpu_scene.hpp"
#include "graphics_context.hpp"
#include "pipeline_builder.hpp"
#include "shaders/shader_loader.hpp"
#include "spdlog/spdlog.h"
#include "vulkan_context.hpp"

#include <tracy/TracyVulkan.hpp>

DOFPass::DOFPass(const std::shared_ptr<GraphicsContext>& context)
    :_context(context)
{
    CreatePipeline();
}

DOFPass::~DOFPass()
{
    vk::Device device = _context->GetVulkanContext()->Device();
    device.destroy(_pipeline);
    device.destroy(_pipelineLayout);
}

void DOFPass::RecordCommands([[maybe_unused]] vk::CommandBuffer commandBuffer, [[maybe_unused]] bb::u32 currentFrame, [[maybe_unused]] const RenderSceneDescription& scene)
{
    TracyVkZone(scene.tracyContext, commandBuffer, "DOF Pass");

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, _pipeline);

    commandBuffer.dispatch(1, 1, 1);
}

void DOFPass::CreatePipeline()
{
    std::vector<std::byte> compSpv = shader::ReadFile("shaders/bin/dof.comp.spv");

    ComputePipelineBuilder pipelineBuilder { _context };
    pipelineBuilder.AddShaderStage(vk::ShaderStageFlagBits::eCompute, compSpv);

    std::tuple<vk::PipelineLayout, vk::Pipeline> result =  pipelineBuilder.BuildPipeline();

    _pipelineLayout = std::get<0>(result);
    _pipeline = std::get<1>(result);
}