import "../modules/scene";

[vk::binding(0, 0)]
ConstantBuffer<Camera> camera;

struct VSInput
{
    float3 pos;
}

struct VSOutput
{
    float4 pos : SV_Position;
}

[shader("vertex")]
VSOutput main_vert(VSInput input) {

    VSOutput out;
    out.pos = mul(camera.VP, float4(input.pos, 1.0));
    return out;
}

[shader("fragment")]
float4 main_frag() {
    return float4(0.0, 1.0, 0.0, 1.0); // Green color
}