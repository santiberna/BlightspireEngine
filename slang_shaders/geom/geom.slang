
import common.scene;
import common.bindless;
import common.octahedron;

[vk::binding(0, 1)]
StructuredBuffer<scene::Instance> InstanceData; // TODO: Rename to InstanceData

[vk::binding(0, 2)]
ConstantBuffer<scene::Camera> CameraUBO; // TODO: Rename to camera

[vk::binding(0, 3)]
StructuredBuffer<uint> RedirectBuffer; // TODO: Rename to redirect

[vk::binding(1, 3)]
StructuredBuffer<uint> RedirectCountBuffer; // TODO: Rename to redirectCount

struct PushConstant
{
    uint isDirectCommand;
    uint directInstanceIndex;
};

[vk::push_constant]
ConstantBuffer<PushConstant> pc; 

/// Gpt o1 random hash function
// A simple 32-bit hash function
uint hash32(uint x) {
    x ^= x >> 16;
    x *= 0x7FEB352D;
    x ^= x >> 15;
    x *= 0x846CA68B;
    x ^= x >> 16;
    return x;
}

// Return a float in [0..1) from the hash
float randomFloatFromCoord(uint2 coord)
{
    // Combine x and y into a single 32-bit integer
    // so that each pixel coordinate produces a unique seed.
    // The shift + XOR helps reduce collisions.
    uint seed = hash32(uint(coord.x) ^ (uint(coord.y) << 16));
    // Map [0 .. 2^32-1] to [0.0 .. 1.0).
    return float(seed) * (1.0 / 4294967296.0);
}

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texCoord : TEXCOOORD0;
    uint drawID : SV_DrawIndex;
};

struct VSOutput
{
    float4 position : SV_Position;
    [vk::location(0)] float3 viewPosition; // TODO: was position
    [vk::location(1)] float3 normal;
    [vk::location(2)] float2 texCoord;
    [vk::location(3)] nointerpolation uint drawID;

    [vk::location(4)] float3 T;
    [vk::location(5)] float3 B;
    [vk::location(6)] float3 N;
};

struct FSOut
{
    float4 albedoRM : SV_Target0; // RGB: Albedo, A: Roughness Metallic
    float4 normal : SV_Target1;   // RG:  Normal
}


float3x3 Adjoint(in float4x4 m)
{
    return float3x3(
    cross(m[1].xyz, m[2].xyz),
    cross(m[2].xyz, m[0].xyz),
    cross(m[0].xyz, m[1].xyz)
    );
}

[shader("vertex")]
VSOutput main_vert(VSInput input)
{
    VSOutput output;
    float4x4 modelTransform;

    if (pc.isDirectCommand == 1)
    {
        modelTransform = InstanceData[pc.directInstanceIndex].model;
    }
    else
    {
        modelTransform = InstanceData[RedirectBuffer[input.drawID]].model;
        output.drawID = RedirectBuffer[input.drawID];
    }

    output.viewPosition = mul(modelTransform, float4(input.position, 1.0)).xyz;

    float3x3 normalTransform = Adjoint(modelTransform);
    output.normal = normalize(mul(normalTransform, input.normal));
    float3 tangent = normalize(mul(normalTransform, input.tangent.xyz));
    float3 bitangent = normalize(mul(normalTransform, (mul(input.tangent.w, cross(input.normal, input.tangent.xyz)))));
    output.T = tangent;
    output.B = bitangent;
    output.N = output.normal;
    output.texCoord = input.texCoord;

    output.position = mul(CameraUBO.VP, float4(output.viewPosition, 1.0));
    float3 viewPos = mul(CameraUBO.view, float4(output.viewPosition, 1.0)).xyz;
    output.viewPosition = viewPos;

    if (pc.isDirectCommand == 1)
    {
        output.position = float4(output.position.x, output.position.y, 1.0, output.position.w);
    }

    return output;
}

[shader("fragment")]
FSOut main_frag(VSOutput input)
{
    FSOut output;

    uint actualDrawID;

    if (pc.isDirectCommand == 1)
    {
        actualDrawID = pc.directInstanceIndex;
    }
    else
    {
        actualDrawID = input.drawID;
    }

    Material material = Materials[InstanceData[actualDrawID].materialIndex]; // TODO: Confirm whether NonUniformIndex is required.

    float4 albedoSample = float4(1.0);
    float4 mrSample = float4(0.0);
    float4 occlusionSample = float4(0.0);
    float4 normalSample = float4(input.normal, 0.0);
    float3 normal = input.normal;

    float alpha = InstanceData[actualDrawID].transparency;

    // If alpha is not fully opaque (1.0), apply noise dithering
    if (alpha < 1.0)
    {
        uint2 pixelPos = uint2(input.position.xy);
        float noiseVal = randomFloatFromCoord(pixelPos);

        // If the transparency is less than this random threshold, discard
        if (alpha < noiseVal)
        {
            discard;
        }
    }

    if (material.useAlbedoMap)
    {
        albedoSample = pow(bindless_color_textures[material.albedoMapIndex].Sample(input.texCoord), float4(2.2)); // TODO: Confirm whether NonUniformIndex is required.
        if (albedoSample.a < 1.0)
        {
            discard;
        }
    }
    if (material.useMRMap)
    {
        mrSample = bindless_color_textures[material.mrMapIndex].Sample(input.texCoord); // TODO: Confirm whether NonUniformIndex is required.
    }
    if (material.useNormalMap)
    {
        normalSample = bindless_color_textures[material.normalMapIndex].Sample(input.texCoord); // TODO: Confirm whether NonUniformIndex is required.
    }
    if (material.useOcclusionMap)
    {
        occlusionSample = bindless_color_textures[material.occlusionMapIndex].Sample(input.texCoord); // TODO: Confirm whether NonUniformIndex is required.
    }

    albedoSample *= material.albedoFactor;
    mrSample *= float4(1.0, material.roughnessFactor, material.metallicFactor, 1.0);
    occlusionSample *= float4(material.occlusionStrength);

    if (material.useNormalMap)
    {
        normal = normalSample.xyz * 2.0 - 1.0;
        normal = normalize(mul(float3x3(input.T, input.B, input.N), normal)) * material.normalScale;
    }

    output.albedoRM = float4(albedoSample.rgb, 0.0);//scene::EncodeRM(clamp(mrSample.g, 0.0, 1.0), clamp(mrSample.b, 0.0, 1.0)));
    output.normal = float4(0.0);//float4(OctEncode(normal), 0.0, 0.0);

    return output;
}
