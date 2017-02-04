// TagBuffer.cpp

#include <Core/Memory/Functions.h>
#include "../include/Engine/TagBuffer.h"

namespace sge
{
    TagBuffer TagBuffer::create(
        const TypeInfo& component_type,
        const EntityId* ord_entities,
        const TagCount_t* tag_counts,
        std::size_t num_ents,
        const void* tag_buffer,
        std::size_t tag_buffer_size)
    {
        TagBuffer result;

        // Initialize the result
        result._component_type = &component_type;
        result._ord_entities.insert(result._ord_entities.begin(), ord_entities, ord_entities + num_ents);
        result._tag_counts.insert(result._tag_counts.begin(), tag_counts, tag_counts + num_ents);

        // Copy in the tags
        result._tag_buffer = sge::malloc(tag_buffer_size);
        std::memcpy(result._tag_buffer, tag_buffer, tag_buffer_size);
        result._tag_buffer_size = tag_buffer_size;

        return result;
    }

    TagBuffer TagBuffer::create_single(
        const TypeInfo& component_type,
        const EntityId* ord_entities,
        std::size_t num_ents,
        const void* tag_buffer,
        std::size_t tag_buffer_size)
    {
        TagBuffer result;

        // Initialize the result
        result._component_type = &component_type;
        result._ord_entities.insert(result._ord_entities.begin(), ord_entities, ord_entities + num_ents);

        // Copy in the tags
        result._tag_buffer = sge::malloc(tag_buffer_size);
        std::memcpy(result._tag_buffer, tag_buffer, tag_buffer_size);
        result._tag_buffer_size = tag_buffer_size;

        return result;
    }

    TagBuffer TagBuffer::create_empty(
        const TypeInfo& component_type,
        const EntityId* ord_entities,
        const TagCount_t* tag_counts,
        std::size_t num_ents)
    {
        TagBuffer result;

        // Initialize the result
        result._component_type = &component_type;
        result._ord_entities.insert(result._ord_entities.begin(), ord_entities, ord_entities + num_ents);
        result._tag_counts.insert(result._tag_counts.begin(), tag_counts, tag_counts + num_ents);

        return result;
    }

    TagBuffer TagBuffer::create_single_empty(
        const TypeInfo& component_type,
        const EntityId* ord_entities,
        std::size_t num_ents)
    {
        TagBuffer result;

        // Initialize the result
        result._component_type = &component_type;
        result._ord_entities.insert(result._ord_entities.begin(), ord_entities, ord_entities + num_ents);

        return result;
    }

    TagBuffer::~TagBuffer()
    {
        sge::free(_tag_buffer);
    }

    TagBuffer::TagBuffer(TagBuffer&& move)
        : _component_type(move._component_type),
        _ord_entities(std::move(move._ord_entities)),
        _tag_counts(std::move(move._tag_counts)),
        _tag_buffer(move._tag_buffer),
        _tag_buffer_size(move._tag_buffer_size)
    {
        move._component_type = nullptr;
        move._tag_buffer = nullptr;
        move._tag_buffer_size = 0;
    }

    TagBuffer::TagBuffer()
        : _component_type(nullptr),
        _tag_buffer(nullptr),
        _tag_buffer_size(0)
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

    std::size_t TagBuffer::get_num_entities() const
    {
        return _ord_entities.size();
    }

    const TagCount_t* TagBuffer::get_tag_counts() const
    {
        if (_tag_counts.empty())
        {
            return nullptr;
        }

        return _tag_counts.data();
    }

    const void* TagBuffer::get_buffer() const
    {
        return _tag_buffer;
    }

    std::size_t TagBuffer::buffer_size() const
    {
        return _tag_buffer_size;
    }

    bool TagBuffer::tags_single() const
    {
        return _tag_counts.empty();
    }

    bool TagBuffer::tags_empty() const
    {
        return _tag_buffer == nullptr;
    }
}
