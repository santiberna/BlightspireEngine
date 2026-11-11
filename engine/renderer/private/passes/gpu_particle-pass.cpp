#include "passes/gpu_particle_effect_pass.hpp"

#include "graphics_context.hpp"
#include "resources/buffer.hpp"
#include "graphics_resources.hpp"

GPUParticleEffectPass::GPUParticleEffectPass(const std::shared_ptr<GraphicsContext>& context)
    :_context(context)
{
    CreateBuffers();
}

void GPUParticleEffectPass::CreateBuffers()
{
    std::shared_ptr<GraphicsResources> resources = _context->Resources();

    for(bb::u8 i = 0; i < PARTICLE_BUFFER_COUNT; i++)
    {
        bb::Flags<bb::BufferFlags> flags = { bb::BufferFlags::STORAGE_USAGE };
        _particleBuffers[i] = resources->GetBufferResourceManager().Create(
            sizeof(Particle) * PARTICLE_BUFFER_SIZE,
            flags,
            std::string("GPU Particle effect buffer ").append(std::to_string(i)).c_str()
        );
    }
}