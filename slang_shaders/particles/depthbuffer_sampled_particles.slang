import common.scene;

struct PushConstant {
    float deltaTime;
}

struct Particle {
    float Value;
}

[vk::binding(0, 0)]
RWStructuredBuffer<Particle> particleBuffer;

[numthreads(64, 1, 1)]
[shader("compute")]
void main(uniform PushConstant pushConstant, uint3 ID : SV_DispatchThreadID)
{
    particleBuffer[ID.x].Value = pushConstant.deltaTime;
}