import common.scene;
import particle;

[vk_binding(0, 0)]
ConstantBuffer<Camera> cameraUBO;

static const float2 quadOffsets[6] = {
    float2(-0.5f, -0.5f),
    float2( 0.5f, -0.5f),
    float2(-0.5f,  0.5f),
    float2(-0.5f,  0.5f),
    float2( 0.5f, -0.5f),
    float2( 0.5f,  0.5f)
};

struct VS_OUT {
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

[shader("vertex")]
VS_OUT vert_main(uint vertexId : SV_VertexId)
{
    VS_OUT out;

    float2 offset = quadOffsets[vertexId];

    float3 cameraUp = float3(cameraUBO.view[0][1], cameraUBO.view[1][1], cameraUBO.view[2][1]);
    float3 cameraRight = float3(cameraUBO.view[0][0], cameraUBO.view[1][0], cameraUBO.view[2][0]);
    
    float3 worldVertexPosition = 
        float3(0.f) + 
        (cameraRight * offset.x) +
        (cameraUp * offset.y);

    out.position = mul(cameraUBO.VP, float4(worldVertexPosition, 1.0f));
    out.uv = offset + 0.5f;

    return out;
}

[shader("pixel")]
void frag_main()
{
    
}