import common.scene;

struct Particle {
    uint32_t Value;
}

[vk::binding(0, 0)]
RWStructuredBuffer<Particle> particleBuffer;

[numthreads(64, 1, 1)]
[shader("compute")]
void main(uint3 ID : SV_DispatchThreadID)
{
    particleBuffer[ID.x].Value = 20;
}