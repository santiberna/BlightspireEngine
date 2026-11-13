import particle;

struct PushConstantArgs {
    uint32_t backBufferIndex;
    uint32_t bufferIndex;
}

[vk_binding(0, 0)]
RWStructuredBuffer<TimedParticle>[2] particleBuffers;

[vk_binding(1, 0)]
RWStructuredBuffer<Atomic<int32_t>>[2] counterBuffers;

[numthreads(1, 1, 1)]
[shader("compute")]
void main_comp(uniform PushConstantArgs pushConstant)
{
    // Compute indirect dispatch argument count from alive particles in current backbuffer
    // ...
    RWStructuredBuffer<TimedParticle> buffer = particleBuffers[pushConstant.backBufferIndex];
    // Set counter to 0
    
    counterBuffers[pushConstant.bufferIndex][0].store(0);
}