module bindless;

[vk::binding(0, 0)]
public Sampler2D bindless_color_textures[];

[vk::binding(0, 0)]
public Sampler2D bindless_depth_textures[];

[vk::binding(0, 0)]
public SamplerCube bindless_cubemap_textures[];

[vk::binding(0, 0)]
public Sampler2DShadow bindless_shadowmap_textures[]; 

[vk::binding(1, 0)]
[vk::image_format("r16f")]
public RWTexture2D<float> bindless_storage_image_r16f; 

// TODO: Definition should be moved
public struct Material
{
    public float4 albedoFactor;

    public float metallicFactor;
    public float roughnessFactor;
    public float normalScale;
    public float occlusionStrength;
    public float3 emissiveFactor;

    public bool useEmissiveMap;
    public bool useAlbedoMap;
    public bool useMRMap;
    public bool useNormalMap;
    public bool useOcclusionMap;

    public uint albedoMapIndex;
    public uint mrMapIndex;
    public uint normalMapIndex;
    public uint occlusionMapIndex;
    public uint emissiveMapIndex;
};

[vk::binding(2, 0)]
public ConstantBuffer<Material[64]> Materials; // TODO: rename to bindless_materials
