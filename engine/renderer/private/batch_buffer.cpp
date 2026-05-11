#include "batch_buffer.hpp"

#include "graphics_context.hpp"
#include "graphics_resources.hpp"
#include "resources/buffer.hpp"
#include "single_time_commands.hpp"

BatchBuffer::BatchBuffer(const std::shared_ptr<GraphicsContext>& context, bb::u32 vertexBufferSize, bb::u32 indexBufferSize)
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

bb::u32 BatchBuffer::AppendIndices(const std::vector<bb::u32>& indices, SingleTimeCommands& commandBuffer)
{
    auto resources { _context->Resources() };

    assert((_indexOffset + indices.size()) * sizeof(bb::u32) < _indexBufferSize);
    bb::u32 originalOffset = _indexOffset;

    const bb::Buffer* buffer = resources->GetBufferResourceManager().Access(_indexBuffer);
    commandBuffer.CopyIntoLocalBuffer(indices, _indexOffset, buffer->buffer);

    _indexOffset += indices.size();

    return originalOffset;
}
