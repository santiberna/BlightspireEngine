#pragma once

#include "common.hpp"
#include "resource_manager.hpp"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <optional>

struct GPUImage;
class ImageResourceManager;

// For now this is only meant to be used in combination with an owning CPUModel.
struct CPUMaterial
{
    // Texture indices corrospond to the index in the array of textures located in the owning CPUModel
    using TextureIndex = bb::u32;
    std::optional<TextureIndex> albedoMap;
    glm::vec4 albedoFactor { 0.0f };
    bb::u32 albedoUVChannel;

    std::optional<TextureIndex> metallicRoughnessMap;
    float metallicFactor { 0.0f };
    float roughnessFactor { 0.0f };
    std::optional<TextureIndex> metallicRoughnessUVChannel;

    std::optional<TextureIndex> normalMap;
    float normalScale { 1.0f };
    bb::u32 normalUVChannel;

    std::optional<TextureIndex> occlusionMap;
    float occlusionStrength { 0.0f };
    bb::u32 occlusionUVChannel;

    std::optional<TextureIndex> emissiveMap;
    glm::vec3 emissiveFactor { 0.0f };
    bb::u32 emissiveUVChannel;
};

struct MaterialCreation
{
    ResourceHandle<GPUImage> albedoMap;
    glm::vec4 albedoFactor { 0.0f };
    bb::u32 albedoUVChannel;

    ResourceHandle<GPUImage> metallicRoughnessMap;
    float metallicFactor { 0.0f };
    float roughnessFactor { 0.0f };
    std::optional<bb::u32> metallicRoughnessUVChannel;

    ResourceHandle<GPUImage> normalMap;
    float normalScale { 0.0f };
    bb::u32 normalUVChannel;

    ResourceHandle<GPUImage> occlusionMap;
    float occlusionStrength { 0.0f };
    bb::u32 occlusionUVChannel;

    ResourceHandle<GPUImage> emissiveMap;
    glm::vec3 emissiveFactor { 0.0f };
    bb::u32 emissiveUVChannel;
};

struct GPUMaterial
{
    GPUMaterial() = default;
    GPUMaterial(const MaterialCreation& creation, const std::shared_ptr<ImageResourceManager>& imageResourceManager);
    ~GPUMaterial();

    GPUMaterial(GPUMaterial&& other) noexcept = default;
    GPUMaterial& operator=(GPUMaterial&& other) noexcept = default;

    NON_COPYABLE(GPUMaterial);

    // Info that gets send to the gpu
    struct alignas(16) GPUInfo
    {
        glm::vec4 albedoFactor { 0.0f };

        float metallicFactor { 0.0f };
        float roughnessFactor { 0.0f };
        float normalScale { 1.0f };
        float occlusionStrength { 0.0f };

        glm::vec3 emissiveFactor { 0.0f };
        bb::i32 useEmissiveMap { false };

        bb::i32 useAlbedoMap { false };
        bb::i32 useMRMap { false };
        bb::i32 useNormalMap { false };
        bb::i32 useOcclusionMap { false };

        bb::u32 albedoMapIndex;
        bb::u32 mrMapIndex;
        bb::u32 normalMapIndex;
        bb::u32 occlusionMapIndex;

        bb::u32 emissiveMapIndex;
    } gpuInfo;

    ResourceHandle<GPUImage> albedoMap;
    ResourceHandle<GPUImage> mrMap;
    ResourceHandle<GPUImage> normalMap;
    ResourceHandle<GPUImage> occlusionMap;
    ResourceHandle<GPUImage> emissiveMap;

private:
    std::shared_ptr<ImageResourceManager> _imageResourceManager;
};