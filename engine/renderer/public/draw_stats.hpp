#pragma once

#include "common.hpp"

class DrawStats
{
public:
    void IndirectDraw(bb::u32 drawCommandSize, bb::u32 indexCount);
    void DrawIndexed(bb::u32 indexCount);
    void Draw(bb::u32 vertexCount);
    void SetParticleCount(bb::u32 particleCount);
    void SetEmitterCount(bb::u32 emitterCount);

    void Clear();

    bb::u32 IndexCount() const { return _indexCount; }
    bb::u32 DrawCalls() const { return _drawCalls; }
    bb::u32 DirectDrawCommands() const { return _directDrawCommands; }
    bb::u32 IndirectDrawCommands() const { return _indirectDrawCommands; }
    bb::u32 GetParticleCount() const { return _particleCount; }
    bb::u32 GetEmitterCount() const { return _emitterCount; }

private:
    bb::u32 _indexCount;
    bb::u32 _drawCalls;
    bb::u32 _directDrawCommands;
    bb::u32 _indirectDrawCommands;
    bb::u32 _particleCount;
    bb::u32 _emitterCount;
};
