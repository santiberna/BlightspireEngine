#include "resources/material.hpp"
#include "resources/texture.hpp"

GPUMaterial::GPUMaterial(const MaterialCreation& creation)
{
    albedoMap = creation.albedoMap;
    mrMap = creation.metallicRoughnessMap;
    normalMap = creation.normalMap;
    occlusionMap = creation.occlusionMap;
    emissiveMap = creation.emissiveMap;

    gpuInfo.useAlbedoMap = static_cast<int32_t>(albedoMap.isValid());
    gpuInfo.useMRMap = static_cast<int32_t>(mrMap.isValid());
    gpuInfo.useNormalMap = static_cast<int32_t>(normalMap.isValid());
    gpuInfo.useOcclusionMap = static_cast<int32_t>(occlusionMap.isValid());
    gpuInfo.useEmissiveMap = static_cast<int32_t>(emissiveMap.isValid());

    gpuInfo.albedoMapIndex = albedoMap.getIndex();
    gpuInfo.mrMapIndex = mrMap.getIndex();
    gpuInfo.normalMapIndex = normalMap.getIndex();
    gpuInfo.occlusionMapIndex = occlusionMap.getIndex();
    gpuInfo.emissiveMapIndex = emissiveMap.getIndex();

    gpuInfo.albedoFactor = creation.albedoFactor;
    gpuInfo.metallicFactor = creation.metallicFactor;
    gpuInfo.roughnessFactor = creation.roughnessFactor;
    gpuInfo.normalScale = creation.normalScale;
    gpuInfo.occlusionStrength = creation.occlusionStrength;
    gpuInfo.emissiveFactor = creation.emissiveFactor;
}

GPUMaterial::~GPUMaterial()
{
}