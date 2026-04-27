#pragma once

#include "common.hpp"
#include "constants.hpp"
#include "graphics_context.hpp"
#include "graphics_resources.hpp"
#include "resources/buffer.hpp"
#include "single_time_commands.hpp"
#include "vertex.hpp"
#include "vulkan_helper.hpp"

#include <cstddef>
#include <memory>
#include <vector>

class BatchBuffer
{
public:
    BatchBuffer(const std::shared_ptr<GraphicsContext>& context, bb::u32 vertexBufferSize, bb::u32 indexBufferSize);
    ~BatchBuffer();
    NON_MOVABLE(BatchBuffer);
    NON_COPYABLE(BatchBuffer);

    ResourceHandle<bb::Buffer> VertexBuffer() const { return _vertexBuffer; }
    ResourceHandle<bb::Buffer> IndexBuffer() const { return _indexBuffer; };
    vk::IndexType IndexType() const { return _indexType; }
    vk::PrimitiveTopology Topology() const { return _topology; }

    bb::u32 VertexBufferSize() const { return _vertexBufferSize; }
    bb::u32 IndexBufferSize() const { return _indexBufferSize; }

    ResourceHandle<bb::Buffer> IndirectDrawBuffer(bb::u32 frameIndex) const { return _indirectDrawBuffers[frameIndex]; }

    template <typename T>
    bb::u32 AppendVertices(const std::vector<T>& vertices, SingleTimeCommands& commandBuffer)
    {
        auto resources { _context->Resources() };

        assert((_vertexOffset + vertices.size()) * sizeof(T) < _vertexBufferSize);
        bb::u32 originalOffset = _vertexOffset;

        const bb::Buffer* buffer = resources->GetBufferResourceManager().Access(_vertexBuffer);
        commandBuffer.CopyIntoLocalBuffer(vertices, _vertexOffset, buffer->buffer);

        _vertexOffset += vertices.size();

        return originalOffset;
    }

    bb::u32 AppendIndices(const std::vector<bb::u32>& indices, SingleTimeCommands& commandBuffer);

private:
    std::shared_ptr<GraphicsContext> _context;

    bb::u32 _vertexBufferSize;
    bb::u32 _indexBufferSize;
    vk::IndexType _indexType;
    vk::PrimitiveTopology _topology;

    ResourceHandle<bb::Buffer> _vertexBuffer;
    ResourceHandle<bb::Buffer> _indexBuffer;
    std::array<ResourceHandle<bb::Buffer>, MAX_FRAMES_IN_FLIGHT> _indirectDrawBuffers;

    bb::u32 _vertexOffset { 0 };
    bb::u32 _indexOffset { 0 };
};