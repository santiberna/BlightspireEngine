import particles.particle;

struct PushConstant {
    float deltaTime;
    uint32_t backBufferIndex;
    uint32_t bufferIndex;
}

[vk::binding(0, 0)]
RWStructuredBuffer<TimedParticle>[2] particleBuffer;

[vk::binding(1, 0)]
RWStructuredBuffer<Atomic<uint32_t>>[2] counter;

[numthreads(64, 1, 1)]
[shader("compute")]
void main_comp(uniform PushConstant pushConstant, uint3 ID : SV_DispatchThreadID)
{
    particleBuffer[pushConstant.bufferIndex][ID.x].Value = pushConstant.deltaTime;
    counter[pushConstant.bufferIndex][0].increment();
}