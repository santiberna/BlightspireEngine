#include "resources/material.hpp"
#include "resources/texture.hpp"

GPUMaterial::GPUMaterial(const MaterialCreation& creation, const std::shared_ptr<ImageResourceManager>& imageResourceManager)
    : _imageResourceManager(imageResourceManager)
{
    albedoMap = creation.albedoMap;
    mrMap = creation.metallicRoughnessMap;
    normalMap = creation.normalMap;
    occlusionMap = creation.occlusionMap;
    emissiveMap = creation.emissiveMap;

    gpuInfo.useAlbedoMap = albedoMap.isValid();
    gpuInfo.useMRMap = mrMap.isValid();
    gpuInfo.useNormalMap = normalMap.isValid();
    gpuInfo.useOcclusionMap = occlusionMap.isValid();
    gpuInfo.useEmissiveMap = emissiveMap.isValid();

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