#include "resource_management/material_resource_manager.hpp"
#include "resource_management/image_resource_manager.hpp"

MaterialResourceManager::MaterialResourceManager(const std::shared_ptr<ImageResourceManager>& imageResourceManager)
    : _imageResourceManager(imageResourceManager)
{
}

ResourceHandle<GPUMaterial> MaterialResourceManager::Create(const MaterialCreation& creation)
{
    GPUMaterial material = GPUMaterial(creation);
    return ResourceManager::Create(std::move(material));
}
