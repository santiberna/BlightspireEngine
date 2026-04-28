#pragma once

#include "camera.hpp"
#include "constants.hpp"
#include "resource_manager.hpp"
#include "resources/image.hpp"
#include "resources/sampler.hpp"
#include "resources/texture.hpp"
#include "settings.hpp"

#include <entt/entity/entity.hpp>
#include <memory>
#include <tracy/TracyVulkan.hpp>

class GPUScene;
class BatchBuffer;
class ECSModule;
class GraphicsContext;
class CameraBatch;

struct GPUSceneCreation
{
    std::shared_ptr<GraphicsContext> context;
    ECSModule& ecs;

    ResourceHandle<GPUImage> irradianceMap;
    ResourceHandle<GPUImage> prefilterMap;
    ResourceHandle<GPUImage> brdfLUTMap;
    ResourceHandle<GPUImage> depthImage;

    glm::uvec2 displaySize;
};

struct RenderSceneDescription
{
    std::shared_ptr<GPUScene> gpuScene;
    ECSModule& ecs;
    std::shared_ptr<BatchBuffer> staticBatchBuffer;
    std::shared_ptr<BatchBuffer> skinnedBatchBuffer;
    bb::u32 targetSwapChainImageIndex;
    float deltaTime;
    TracyVkCtx& tracyContext;
};

constexpr bb::u32 MAX_STATIC_INSTANCES = 1 << 14;
constexpr bb::u32 MAX_SKINNED_INSTANCES = 1 << 13;
constexpr bb::u32 MAX_POINT_LIGHTS = 1 << 13;
constexpr bb::u32 MAX_BONES = 1 << 14;
constexpr bb::u32 MAX_LIGHTS_PER_CLUSTER = 256;
constexpr bb::u32 MAX_DECALS = 32;

constexpr bb::u32 CLUSTER_X = 16, CLUSTER_Y = 9, CLUSTER_Z = 24;
constexpr bb::u32 CLUSTER_SIZE = CLUSTER_X * CLUSTER_Y * CLUSTER_Z;

struct DrawIndexedIndirectCommand
{
    vk::DrawIndexedIndirectCommand command;
};

struct DrawIndexedDirectCommand
{
    bb::u32 instanceIndex {};
    bb::u32 indexCount {};
    bb::u32 firstIndex {};
    bb::i32 vertexOffset {};
};

class GPUScene
{
public:
    GPUScene(const GPUSceneCreation& creation, const Settings::Fog& settings);
    ~GPUScene();

    NON_COPYABLE(GPUScene);
    NON_MOVABLE(GPUScene);

    void Update(bb::u32 frameIndex);
    void UpdateGlobalIndexBuffer(vk::CommandBuffer& commandBuffer);

    void SpawnDecal(glm::vec3 normal, glm::vec3 position, glm::vec2 size, std::string albedoName);
    void ResetDecals();

    const vk::DescriptorSet& GetSceneDescriptorSet(bb::u32 frameIndex) const { return _sceneFrameData.at(frameIndex).descriptorSet; }
    const vk::DescriptorSet& GetStaticInstancesDescriptorSet(bb::u32 frameIndex) const { return _staticInstancesFrameData.at(frameIndex).descriptorSet; }
    const vk::DescriptorSet& GetSkinnedInstancesDescriptorSet(bb::u32 frameIndex) const { return _skinnedInstancesFrameData.at(frameIndex).descriptorSet; }
    const vk::DescriptorSet& GetPointLightDescriptorSet(bb::u32 frameIndex) const { return _pointLightFrameData.at(frameIndex).descriptorSet; }
    const vk::DescriptorSet& GetClusterDescriptorSet() const { return _clusterData.descriptorSet; }
    const vk::DescriptorSet& GetClusterCullingDescriptorSet(bb::u32 frameIndex) const { return _clusterCullingData.descriptorSets.at(frameIndex); }
    const vk::DescriptorSet& GetDecalDescriptorSet(bb::u32 frameIndex) const { return _decalFrameData[frameIndex].descriptorSet; }
    const vk::DescriptorSetLayout& GetSceneDescriptorSetLayout() const { return _sceneDescriptorSetLayout; }
    const vk::DescriptorSetLayout& GetObjectInstancesDescriptorSetLayout() const { return _objectInstancesDSL; }
    const vk::DescriptorSetLayout& GetPointLightDescriptorSetLayout() const { return _pointLightDSL; }
    const vk::DescriptorSetLayout& GetHZBDescriptorSetLayout() const { return _hzbImageDSL; }
    const vk::DescriptorSetLayout& GetClusterDescriptorSetLayout() const { return _clusterDescriptorSetLayout; }
    const vk::DescriptorSetLayout& GetClusterCullingDescriptorSetLayout() const { return _clusterCullingDescriptorSetLayout; }
    const vk::DescriptorSetLayout& GetDecalDescriptorSetLayout() const { return _decalDescriptorSetLayout; }

    vk::DescriptorSetLayout DrawBufferLayout() const { return _drawBufferDSL; }
    ResourceHandle<bb::Buffer> StaticDrawBuffer(bb::u32 frameIndex) const { return _staticDraws[frameIndex].buffer; }
    vk::DescriptorSet StaticDrawDescriptorSet(bb::u32 frameIndex) const { return _staticDraws[frameIndex].descriptorSet; }
    ResourceHandle<bb::Buffer> SkinnedDrawBuffer(bb::u32 frameIndex) const { return _skinnedDraws[frameIndex].buffer; }
    vk::DescriptorSet SkinnedDrawDescriptorSet(bb::u32 frameIndex) const { return _skinnedDraws[frameIndex].descriptorSet; }

    const vk::DescriptorSetLayout GetSkinDescriptorSetLayout() const { return _skinDescriptorSetLayout; }
    const vk::DescriptorSet GetSkinDescriptorSet(bb::u32 frameIndex) const { return _skinDescriptorSets[frameIndex]; }

    bb::u32 StaticDrawCount() const { return _staticDrawCommands.size(); };
    const std::vector<DrawIndexedIndirectCommand>& StaticDrawCommands() const { return _staticDrawCommands; }
    const std::vector<DrawIndexedDirectCommand>& ForegroundStaticDrawCommands() const { return _foregroundStaticDrawCommands; }
    ResourceHandle<bb::Buffer>& GetClusterBuffer() { return _clusterData.buffer; }
    ResourceHandle<bb::Buffer>& GetClusterCullingBuffer(bb::u32 index) { return _clusterCullingData.buffers.at(index); }
    ResourceHandle<bb::Buffer>& GetGlobalIndexBuffer() { return _clusterCullingData.globalIndexBuffer; }

    bb::u32 SkinnedDrawCount() const { return _skinnedDrawCommands.size(); };
    const std::vector<DrawIndexedIndirectCommand>& SkinnedDrawCommands() const { return _skinnedDrawCommands; }
    const std::vector<DrawIndexedDirectCommand>& ForegroundSkinnedDrawCommands() const { return _foregroundSkinnedDrawCommands; }
    bb::u32 DrawCommandIndexCount(std::vector<DrawIndexedIndirectCommand> commands) const
    {
        bb::u32 count { 0 };
        for (bb::usize i = 0; i < commands.size(); ++i)
        {
            const auto& command = commands[i];
            count += command.command.indexCount;
        }
        return count;
    }

    const CameraResource& MainCamera() const { return _mainCamera; }
    CameraBatch& MainCameraBatch() const { return *_mainCameraBatch; }

    const CameraResource& ForegroundCamera() const { return _foregroundCamera; }

    ResourceHandle<GPUImage> StaticShadow() const { return _staticShadowImage; }
    ResourceHandle<GPUImage> DynamicShadow() const { return _dynamicShadowImage; }
    bool ShouldUpdateShadows() const { return _shouldUpdateShadows; }

    const CameraResource& DirectionalLightShadowCamera() const { return _directionalLightShadowCamera; }
    CameraBatch& ShadowCameraBatch() const { return *_shadowCameraBatch; }

    ResourceHandle<GPUImage> irradianceMap;
    ResourceHandle<GPUImage> prefilterMap;
    ResourceHandle<GPUImage> brdfLUTMap;

    float& FogDensity() { return _sceneData.fogDensity; }

private:
    struct alignas(16) DirectionalLightData
    {
        glm::mat4 lightVP {};
        glm::mat4 depthBiasMVP {};

        glm::vec4 direction {};
        glm::vec4 color {};
        float poissonWorldOffset {};
        float poissonConstant {};
        bb::u32 padding[2];
    };

    struct alignas(16) PointLightData
    {
        glm::vec3 position {};
        float range {};
        glm::vec3 color {};
        float intensity {};
    };

    struct alignas(16) PointLightArray
    {
        std::array<PointLightData, MAX_POINT_LIGHTS> lights {};
        bb::u32 count {};
    };

    struct alignas(16) SceneData
    {
        DirectionalLightData directionalLight {};

        bb::u32 irradianceIndex {};
        bb::u32 prefilterIndex {};
        bb::u32 brdfLUTIndex {};
        bb::u32 staticShadowMapIndex {};

        bb::u32 dynamicShadowMapIndex {};
        float fogDensity {};
        bb::u32 padding[2];

        glm::vec3 fogColor {};

    } _sceneData;

    struct InstanceData
    {
        glm::mat4 model {};

        bb::u32 materialIndex {};
        float boundingRadius {};
        bb::u32 boneOffset {};
        bool isStaticDraw {};
        float transparency = 1.0f;
        bb::u32 padding[3];
    };

    struct alignas(16) DecalData
    {
        glm::mat4 invModel {};
        glm::vec3 orientation {};
        bb::u32 albedoIndex {};
    };

    struct alignas(16) DecalArray
    {
        std::array<DecalData, MAX_DECALS> decals {};
        bb::u32 count {};
    };

    struct FrameData
    {
        ResourceHandle<bb::Buffer> buffer {};
        vk::DescriptorSet descriptorSet {};
    };

    struct PointLightFrameData
    {
        ResourceHandle<bb::Buffer> buffer {};
        vk::DescriptorSet descriptorSet {};
    };

    struct ClusterData
    {
        ResourceHandle<bb::Buffer> buffer {};
        vk::DescriptorSet descriptorSet {};
    };

    struct ClusterCullingData
    {
        std::array<ResourceHandle<bb::Buffer>, 2> buffers {};
        ResourceHandle<bb::Buffer> globalIndexBuffer {};
        std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptorSets {};
    };

    std::shared_ptr<GraphicsContext> _context;
    const Settings::Fog& _settings;
    ECSModule& _ecs;

    vk::DescriptorSetLayout _sceneDescriptorSetLayout;
    std::array<FrameData, MAX_FRAMES_IN_FLIGHT> _sceneFrameData;
    vk::DescriptorSetLayout _objectInstancesDSL;
    std::array<FrameData, MAX_FRAMES_IN_FLIGHT> _staticInstancesFrameData;
    std::array<FrameData, MAX_FRAMES_IN_FLIGHT> _skinnedInstancesFrameData;
    vk::DescriptorSetLayout _drawBufferDSL;
    std::array<FrameData, MAX_FRAMES_IN_FLIGHT> _staticDraws;
    std::array<FrameData, MAX_FRAMES_IN_FLIGHT> _skinnedDraws;
    vk::DescriptorSetLayout _pointLightDSL;
    std::array<PointLightFrameData, MAX_FRAMES_IN_FLIGHT> _pointLightFrameData;

    vk::DescriptorSetLayout _clusterDescriptorSetLayout;
    ClusterData _clusterData;

    vk::DescriptorSetLayout _clusterCullingDescriptorSetLayout;
    ClusterCullingData _clusterCullingData;

    DecalArray _decals {};
    bb::u32 _decalIndex {};

    ResourceHandle<GPUImage>& GetDecalImage(std::string fileName);
    std::unordered_map<std::string, ResourceHandle<GPUImage>> _decalImages;
    std::array<FrameData, MAX_FRAMES_IN_FLIGHT> _decalFrameData;
    vk::DescriptorSetLayout _decalDescriptorSetLayout;

    std::vector<DrawIndexedIndirectCommand> _staticDrawCommands;
    std::vector<DrawIndexedIndirectCommand> _skinnedDrawCommands;
    std::vector<DrawIndexedDirectCommand> _foregroundStaticDrawCommands;
    std::vector<DrawIndexedDirectCommand> _foregroundSkinnedDrawCommands;
    bool _shouldUpdateShadows = false;

    CameraResource _mainCamera;
    CameraResource _foregroundCamera;
    CameraResource _directionalLightShadowCamera;

    std::unique_ptr<CameraBatch> _mainCameraBatch;
    std::unique_ptr<CameraBatch> _shadowCameraBatch;

    ResourceHandle<GPUImage> _staticShadowImage;
    ResourceHandle<GPUImage> _dynamicShadowImage;
    ResourceHandle<bb::Sampler> _shadowSampler;

    vk::DescriptorSetLayout _skinDescriptorSetLayout;
    std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> _skinDescriptorSets;
    std::array<ResourceHandle<bb::Buffer>, MAX_FRAMES_IN_FLIGHT> _skinTransformBuffers;

    vk::DescriptorSetLayout _visibilityDSL;
    vk::DescriptorSetLayout _redirectDSL;
    vk::DescriptorSetLayout _hzbImageDSL;

    std::unordered_map<entt::entity, bb::u32> _skeletonBoneOffset {};

    void UpdateSceneData(bb::u32 frameIndex);
    void UpdatePointLightArray(bb::u32 frameIndex);
    void UpdateObjectInstancesData(bb::u32 frameIndex);
    void UpdateDirectionalLightData(SceneData& scene, bb::u32 frameIndex);
    void UpdatePointLightData(PointLightArray& pointLightArray, bb::u32 frameIndex);
    void UpdateCameraData(bb::u32 frameIndex);
    void UpdateSkinBuffers(bb::u32 frameIndex);
    void UpdateDecalBuffer(bb::u32 frameIndex);

    void InitializeSceneBuffers();
    void InitializePointLightBuffer();
    void InitializeClusterBuffer();
    void InitializeClusterCullingBuffers();
    void InitializeObjectInstancesBuffers();
    void InitializeSkinBuffers();
    void InitializeDecalBuffer();

    void CreateSceneDescriptorSetLayout();
    void CreatePointLightDescriptorSetLayout();
    void CreateClusterDescriptorSetLayout();
    void CreateClusterCullingDescriptorSetLayout();
    void CreateObjectInstanceDescriptorSetLayout();
    void CreateSkinDescriptorSetLayout();
    void CreateHZBDescriptorSetLayout();
    void CreateDecalDescriptorSetLayout();

    void CreateSceneDescriptorSets();
    void CreatePointLightDescriptorSets();
    void CreateClusterDescriptorSet();
    void CreateClusterCullingDescriptorSet();
    void CreateObjectInstancesDescriptorSets();
    void CreateSkinDescriptorSets();
    void CreateDecalDescriptorSets();

    void UpdateSceneDescriptorSet(bb::u32 frameIndex);
    void UpdatePointLightDescriptorSet(bb::u32 frameIndex);
    void UpdateAtomicGlobalDescriptorSet(bb::u32 frameIndex);
    void UpdateObjectInstancesDescriptorSet(bb::u32 frameIndex);
    void UpdateSkinDescriptorSet(bb::u32 frameIndex);
    void UpdateDecalDescriptorSet(bb::u32 frameIndex);

    void CreateSceneBuffers();
    void CreatePointLightBuffer();
    void CreateClusterBuffer();
    void CreateClusterCullingBuffers();
    void CreateObjectInstancesBuffers();
    void CreateSkinBuffers();
    void CreateDecalBuffer();

    void InitializeIndirectDrawBuffer();
    void InitializeIndirectDrawDescriptor();

    void WriteDraws(bb::u32 frameIndex);

    void CreateShadowMapResources();
};
