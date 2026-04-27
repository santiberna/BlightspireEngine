#include "draw_stats.hpp"

#include <cstdint>

void DrawStats::IndirectDraw(bb::u32 drawCommandSize, bb::u32 indexCount)
{
    _drawCalls++;
    _indirectDrawCommands += drawCommandSize;
    _indexCount += indexCount;
}

void DrawStats::DrawIndexed(bb::u32 indexCount)
{
    _directDrawCommands++;
    _drawCalls++;
    _indexCount += indexCount;
}

void DrawStats::Draw(bb::u32 vertexCount)
{
    _drawCalls++;
    _indexCount += vertexCount;
}

void DrawStats::SetParticleCount(bb::u32 particleCount)
{
    _particleCount = particleCount;
}

void DrawStats::SetEmitterCount(bb::u32 emitterCount)
{
    _emitterCount = emitterCount;
}

void DrawStats::Clear()
{
    _drawCalls = 0;
    _indexCount = 0;
    _directDrawCommands = 0;
    _indirectDrawCommands = 0;
    _particleCount = 0;
    _emitterCount = 0;
}
