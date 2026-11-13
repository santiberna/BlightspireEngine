import particles.particle;

struct PushConstant {
    float deltaTime;
    uint32_t backBufferIndex;
    uint32_t bufferIndex;
}

[vk::binding(0, 0)]
RWStructuredBuffer<TimedParticle>[2] particleBuffer;

[vk::binding(1, 0)]
RWStructuredBuffer<Atomic<int32_t>>[2] counter;

[numthreads(64, 1, 1)]
[shader("compute")]
void main_comp(uniform PushConstant pushConstant, uint3 ID : SV_DispatchThreadID)
{
    int32_t backBufferCount =  counter[pushConstant.backBufferIndex][0].load();
    if(ID.x < backBufferCount) // ID does not fall outside of total particle buffer range
    {
        float Value = particleBuffer[pushConstant.backBufferIndex][ID.x].Value;
        Value -= pushConstant.deltaTime;
        if(Value > 0.0f)
        {
            int32_t index = counter[pushConstant.bufferIndex][0].increment();
            if(index < 64)
            {
                particleBuffer[pushConstant.bufferIndex][index].Value = Value;
            }
        }
    }
}