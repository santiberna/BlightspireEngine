#include "batch_buffer.hpp"

#include "graphics_context.hpp"
#include "graphics_resources.hpp"
#include "resources/buffer.hpp"
#include "single_time_commands.hpp"

BatchBuffer::BatchBuffer(const std::shared_ptr<GraphicsContext>& context, uint32_t vertexBufferSize, uint32_t indexBufferSize)
    : _context(context)
    , _vertexBufferSize(vertexBufferSize)
    , _indexBufferSize(indexBufferSize)
    , _indexType(vk::IndexType::eUint32)
    , _topology(vk::PrimitiveTopology::eTriangleList)
{
    auto& buffers { _context->Resources()->GetBufferResourceManager() };

    _vertexBuffer = buffers.Create(
        vertexBufferSize, { bb::BufferFlags::VERTEX_USAGE, bb::BufferFlags::TRANSFER_DST }, "Unified Vertex Buffer");

    _indexBuffer = buffers.Create(
        indexBufferSize, { bb::BufferFlags::INDEX_USAGE, bb::BufferFlags::TRANSFER_DST }, "Unified Index buffer");
}

BatchBuffer::~BatchBuffer()
{
    auto resources { _context->Resources() };
}

uint32_t BatchBuffer::AppendIndices(const std::vector<uint32_t>& indices, SingleTimeCommands& commandBuffer)
{
    auto resources { _context->Resources() };

    assert((_indexOffset + indices.size()) * sizeof(uint32_t) < _indexBufferSize);
    uint32_t originalOffset = _indexOffset;

    const bb::Buffer* buffer = resources->GetBufferResourceManager().Access(_indexBuffer);
    commandBuffer.CopyIntoLocalBuffer(indices, _indexOffset, buffer->buffer);

    _indexOffset += indices.size();

    return originalOffset;
}
