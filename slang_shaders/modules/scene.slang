module scene;

#define MAX_POINT_LIGHTS 8192
#define MAX_DECALS 32

public struct Camera
{
    public float4x4 VP;
    public float4x4 view;
    public float4x4 proj;
    public float4x4 skydomeMVP;
    public float4x4 inverseView;
    public float4x4 inverseProj;
    public float4x4 inverseVP;
    public float3 cameraPosition;
    public int distanceCullingEnabled;
    public float4 frustum;
    public float zNear;
    public float zFar;
    public int cullingEnabled;
    public int projectionType;

    public float fov;
    public float _padding;
};

public struct DirectionalLight
{
    float4x4 lightVP;
    float4x4 depthBiasMVP;

    float4 direction;
    float4 color;
    float poissonWorldOffset;
    float poissonConstant;
};

public struct PointLight
{
    float3 position;
    float range;
    float3 color;
    float intensity;
};

public struct PointLightArray
{
    PointLight lights[MAX_POINT_LIGHTS];
    uint count;
};

public struct Decal
{
    float4x4 invModel;
    float3 orientation;
    uint albedoIndex;
};

public struct DecalArray
{
    Decal decals[MAX_DECALS];
    uint count;
};

public struct Scene
{
    DirectionalLight directionalLight;

    uint irradianceIndex;
    uint prefilterIndex;
    uint brdfLUTIndex;
    uint staticShadowMapIndex;

    uint dynamicShadowMapIndex;
    float fogDensity;
    float3 fogColor;
};

public struct Instance
{
    float4x4 model;
    uint materialIndex;
    float boundingRadius;
    uint boneOffset;
    bool isStaticDraw;
    float transparency;
    float padding[3];
};

static const float2 poissonDisk[16] = float2[](
    float2(-0.94201624, -0.39906216),
    float2(0.94558609, -0.76890725),
    float2(-0.094184101, -0.92938870),
    float2(0.34495938, 0.29387760),
    float2(-0.91588581, 0.45771432),
    float2(-0.81544232, -0.87912464),
    float2(-0.38277543, 0.27676845),
    float2(0.97484398, 0.75648379),
    float2(0.44323325, -0.97511554),
    float2(0.53742981, -0.47373420),
    float2(-0.26496911, -0.41893023),
    float2(0.79197514, 0.19090188),
    float2(-0.24188840, 0.99706507),
    float2(-0.81409955, 0.91437590),
    float2(0.19984126, 0.78641367),
    float2(0.14383161, -0.14100790)
);

float randomIndex(float3 seed, int i)
{
    float4 seed4 = float4(seed, i);
    float dot_product = dot(seed4, float4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float3 ReconstructViewPosition(in float depth, in float2 screenUv, in float4x4 invProj)
{
    float2 ndc = screenUv * 2.0 - 1.0;
    float4 clipSpacePos = float4(ndc, depth, 1.0);
    float4 viewSpacePos = mul(invProj, clipSpacePos);
    viewSpacePos /= viewSpacePos.w;

    return viewSpacePos.xyz;
}

float3 ReconstructWorldPosition(in float depth, in float2 screenUv, in float4x4 invVP)
{
    float2 ndc = screenUv * 2.0 - 1.0;
    float4 clipSpacePos = float4(ndc, depth, 1.0);
    float4 worldSpacePos = mul(invVP, clipSpacePos);
    worldSpacePos /= worldSpacePos.w;

    return worldSpacePos.xyz;
}

static const uint ROUGHNESS_BITS = 5u;
static const uint METALLIC_BITS = 3u;
static const uint ROUGHNESS_RANGE = (1u << ROUGHNESS_BITS) - 1u;
static const uint METALLIC_RANGE = (1u << METALLIC_BITS) - 1u;

float EncodeRM(float roughness, float metallic)
{
    uint r = uint(roughness * ROUGHNESS_RANGE) & ROUGHNESS_RANGE;
    uint m = uint(metallic * METALLIC_RANGE) & METALLIC_RANGE;
    uint c = (r << METALLIC_BITS) | m;
    return float(c) * (1.0 / 255.0);
}

void DecodeRM(float pack, out float roughness, out float metallic)
{
    uint c = uint(pack * 255.0);
    uint r = (c >> METALLIC_BITS) & ROUGHNESS_RANGE;
    uint m = c & METALLIC_RANGE;
    roughness = float(r) / ROUGHNESS_RANGE;
    metallic = float(m) / METALLIC_RANGE;
}
