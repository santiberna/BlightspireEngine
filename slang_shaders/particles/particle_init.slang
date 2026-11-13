import particle;

struct PushConstantArgs {
    uint32_t backBufferIndex;
    uint32_t bufferIndex;
}

[vk_binding(0, 0)]
RWStructuredBuffer<TimedParticle>[2] particleBuffers;

[vk_binding(1, 0)]
RWStructuredBuffer<Atomic<uint32_t>>[2] counterBuffers;

[shader("compute")]
void main_comp(uniform PushConstantArgs pushConstant)
{
    // Compute indirect dispatch argument count from alive particles in current backbuffer
    // ...
    particleBuffers[pushConstant.backBufferIndex][0].Value = 50.0f;
    // Set counter to 0
    counterBuffers[0][0].store(0);
}