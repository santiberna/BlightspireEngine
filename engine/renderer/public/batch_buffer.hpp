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
    BatchBuffer(const std::shared_ptr<GraphicsContext>& context, uint32_t vertexBufferSize, uint32_t indexBufferSize);
    ~BatchBuffer();
    NON_MOVABLE(BatchBuffer);
    NON_COPYABLE(BatchBuffer);

    ResourceHandle<bb::Buffer> VertexBuffer() const { return _vertexBuffer; }
    ResourceHandle<bb::Buffer> IndexBuffer() const { return _indexBuffer; };
    vk::IndexType IndexType() const { return _indexType; }
    vk::PrimitiveTopology Topology() const { return _topology; }

    uint32_t VertexBufferSize() const { return _vertexBufferSize; }
    uint32_t IndexBufferSize() const { return _indexBufferSize; }

    ResourceHandle<bb::Buffer> IndirectDrawBuffer(uint32_t frameIndex) const { return _indirectDrawBuffers[frameIndex]; }

    template <typename T>
    uint32_t AppendVertices(const std::vector<T>& vertices, SingleTimeCommands& commandBuffer)
    {
        auto resources { _context->Resources() };

        assert((_vertexOffset + vertices.size()) * sizeof(T) < _vertexBufferSize);
        uint32_t originalOffset = _vertexOffset;

        const bb::Buffer* buffer = resources->GetBufferResourceManager().Access(_vertexBuffer);
        commandBuffer.CopyIntoLocalBuffer(vertices, _vertexOffset, buffer->buffer);

        _vertexOffset += vertices.size();

        return originalOffset;
    }

    uint32_t AppendIndices(const std::vector<uint32_t>& indices, SingleTimeCommands& commandBuffer);

private:
    std::shared_ptr<GraphicsContext> _context;

    uint32_t _vertexBufferSize;
    uint32_t _indexBufferSize;
    vk::IndexType _indexType;
    vk::PrimitiveTopology _topology;

    ResourceHandle<bb::Buffer> _vertexBuffer;
    ResourceHandle<bb::Buffer> _indexBuffer;
    std::array<ResourceHandle<bb::Buffer>, MAX_FRAMES_IN_FLIGHT> _indirectDrawBuffers;

    uint32_t _vertexOffset { 0 };
    uint32_t _indexOffset { 0 };
};