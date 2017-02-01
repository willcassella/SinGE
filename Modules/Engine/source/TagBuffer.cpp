// TagBuffer.cpp

#include <Core/Memory/Functions.h>
#include "../include/Engine/TagBuffer.h"

namespace sge
{
    TagBuffer TagBuffer::create(
        const TypeInfo& component_type,
        const EntityId* ord_entities,
        const void* tags,
        std::size_t tag_size,
        std::size_t num_tags)
    {
        TagBuffer result;

        // Initialize the result
        result._component_type = &component_type;
        result._ord_entities.insert(result._ord_entities.begin(), ord_entities, ord_entities + num_tags);
        result._size = tag_size * num_tags;
        result._buffer = sge::malloc(result._size);

        // Copy the tags into the buffer
        std::memcpy(result._buffer, tags, result._size);

        return result;
    }

    TagBuffer TagBuffer::create_from_single(
        const TypeInfo& component_type,
        const EntityId* ord_entities,
        const void* single_tag,
        std::size_t tag_size,
        std::size_t num_tags)
    {
        TagBuffer result;

        // Initialize the result
        result._component_type = &component_type;
        result._ord_entities.insert(result._ord_entities.begin(), ord_entities, ord_entities + num_tags);
        result._size = tag_size * num_tags;
        result._buffer = sge::malloc(result._size);

        // Copy the tag into the buffer
        for (std::size_t i = 0; i < num_tags; ++i)
        {
            std::memcpy(static_cast<byte*>(result._buffer) + tag_size * i, single_tag, tag_size);
        }

        return result;
    }

    TagBuffer::~TagBuffer()
    {
        sge::free(_buffer);
    }

    TagBuffer::TagBuffer(TagBuffer&& move)
        : _component_type(move._component_type),
        _ord_entities(std::move(move._ord_entities)),
        _buffer(move._buffer),
        _size(move._size)
    {
        move._component_type = nullptr;
        move._buffer = nullptr;
        move._size = 0;
    }

    TagBuffer::TagBuffer()
        : _component_type(nullptr),
        _buffer(nullptr),
        _size(0)
    {
    }

    const TypeInfo& TagBuffer::component_type() const
    {
        return *_component_type;
    }

    const EntityId* TagBuffer::get_ord_entities() const
    {
        return _ord_entities.data();
    }

    std::size_t TagBuffer::num_tags() const
    {
        return _ord_entities.size();
    }

    const void* TagBuffer::get_buffer() const
    {
        return _buffer;
    }

    std::size_t TagBuffer::buffer_size() const
    {
        return _size;
    }
}
