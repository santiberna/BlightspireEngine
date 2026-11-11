#pragma once

#include "resource_manager.hpp"

#include <memory>
#include <array>

class GraphicsContext;

class GPUParticleEffectPass
{
public:
    GPUParticleEffectPass(const std::shared_ptr<GraphicsContext>& context);

private:
    void CreateBuffers();

private:
    static constexpr bb::u8 PARTICLE_BUFFER_COUNT = 2;
    static constexpr bb::u32 PARTICLE_BUFFER_SIZE = 16;

    std::shared_ptr<GraphicsContext> _context = {};

    std::array<ResourceHandle<bb::Buffer>, PARTICLE_BUFFER_COUNT> _particleBuffers;

    struct Particle {
        bb::u32 value;
    };
};