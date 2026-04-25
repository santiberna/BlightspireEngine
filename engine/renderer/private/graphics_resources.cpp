#include "graphics_resources.hpp"

#include "resource_management/image_resource_manager.hpp"
#include "resource_management/material_resource_manager.hpp"
#include "resource_management/mesh_resource_manager.hpp"
#include "resource_management/model_resource_manager.hpp"
#include "resource_management/sampler_resource_manager.hpp"
#include "resources/buffer.hpp"

GraphicsResources::GraphicsResources(const std::shared_ptr<VulkanContext>& vulkanContext)
    : _vulkanContext(vulkanContext)
{
    _samplerResourceManager = std::make_shared<SamplerResourceManager>(_vulkanContext);
    _samplerResourceManager->CreateDefaultSampler();
    _bufferResourceManager = std::make_shared<bb::BufferManager>(*_vulkanContext);
    _imageResourceManager = std::make_shared<ImageResourceManager>(_vulkanContext, _samplerResourceManager->GetDefaultSamplerHandle());
    _materialResourceManager = std::make_shared<MaterialResourceManager>(_imageResourceManager);
    _meshResourceManager = std::make_shared<MeshResourceManager>(_vulkanContext);
    _modelResourceManager = std::make_shared<ModelResourceManager>(_vulkanContext, _imageResourceManager, _materialResourceManager, _meshResourceManager);
}

GraphicsResources::~GraphicsResources()
{
    _meshResourceManager.reset();
    _materialResourceManager.reset();
    _imageResourceManager.reset();
    _bufferResourceManager.reset();
    _samplerResourceManager.reset();
}
