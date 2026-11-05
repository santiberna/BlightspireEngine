import common.bindless;

static float2 positions[6] = float2[]
(
    float2(0.0, 0.0),
    float2(0.0, 1.0),
    float2(1.0, 1.0),
    float2(1.0, 1.0),
    float2(1.0, 0.0),
    float2(0.0, 0.0)
);

struct QuadDrawInfo
{
    float4x4 matrix;// 64 bytes, aligned to 16 bytes
    float4 color;// 16 bytes, aligned to 16 bytes
    float2 uvMin;// 8 bytes, aligned to 8 bytes
    float2 uvMax;// 8 bytes, aligned to 8 bytes
    uint textureIndex;// 4 bytes, aligned to 4 bytes
    bool useRedAsAlpha;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
}

[vk::push_constant]
ConstantBuffer<QuadDrawInfo> quad;

[shader("vertex")]
VSOutput main_vert(uint vertexID : SV_VertexID) 
{
    VSOutput out;
    out.pos = mul(quad.matrix, float4(positions[vertexID], 1.0, 1.0));

    out.uv.x = lerp(quad.uvMin.x, quad.uvMax.x, positions[vertexID].x);
    out.uv.y = lerp(quad.uvMin.y, quad.uvMax.y, positions[vertexID].y);

    return out;
}

[shader("fragment")]
float4 main_frag(VSOutput in) : SV_Target0
{
    if (quad.useRedAsAlpha)
    {
        return float4(bindless_color_textures[quad.textureIndex].Sample(in.uv).r) * quad.color;
    }
    else
    {
        return bindless_color_textures[quad.textureIndex].Sample(in.uv) * quad.color;
    }
}