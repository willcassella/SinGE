// TagBuffer.cpp

#include <Core/Memory/Functions.h>
#include "../include/Engine/TagBuffer.h"

namespace sge
{
    TagBuffer TagBuffer::create(
        const TypeInfo* component_type,
        const EntityId* ent_range,
        const TagIndex_t* tag_indices,
        const TagCount_t* tag_counts,
        std::size_t ent_range_len,
        const void* tag_buffer,
        std::size_t tag_buffer_size)
    {
        TagBuffer result;

        // Initialize the result
        result._component_type = component_type;
        result._ent_range.insert(result._ent_range.begin(), ent_range, ent_range + ent_range_len);
        result._tag_indices.insert(result._tag_indices.begin(), tag_indices, tag_indices + ent_range_len);
        result._tag_counts.insert(result._tag_counts.begin(), tag_counts, tag_counts + ent_range_len);

        // Copy in the tags
        result._tag_buffer = sge::malloc(tag_buffer_size);
        std::memcpy(result._tag_buffer, tag_buffer, tag_buffer_size);
        result._tag_buffer_size = tag_buffer_size;

        return result;
    }

    TagBuffer TagBuffer::create_single(
        const TypeInfo* component_type,
        const EntityId* ent_range,
        std::size_t ent_range_len,
        const void* tag_buffer,
        std::size_t tag_buffer_size)
    {
        TagBuffer result;

        // Initialize the result
        result._component_type = component_type;
        result._ent_range.insert(result._ent_range.begin(), ent_range, ent_range + ent_range_len);

        // Copy in the tags
        result._tag_buffer = sge::malloc(tag_buffer_size);
        std::memcpy(result._tag_buffer, tag_buffer, tag_buffer_size);
        result._tag_buffer_size = tag_buffer_size;

        return result;
    }

    TagBuffer TagBuffer::create_empty(
        const TypeInfo* component_type,
        const EntityId* ent_range,
        const TagCount_t* tag_counts,
        std::size_t ent_range_len)
    {
        TagBuffer result;

        // Initialize the result
        result._component_type = component_type;
        result._ent_range.insert(result._ent_range.begin(), ent_range, ent_range + ent_range_len);
        result._tag_counts.insert(result._tag_counts.begin(), tag_counts, tag_counts + ent_range_len);

        return result;
    }

    TagBuffer TagBuffer::create_single_empty(
        const TypeInfo* component_type,
        const EntityId* ord_entities,
        std::size_t num_ents)
    {
        TagBuffer result;

        // Initialize the result
        result._component_type = component_type;
        result._ent_range.insert(result._ent_range.begin(), ord_entities, ord_entities + num_ents);

        return result;
    }

    TagBuffer::~TagBuffer()
    {
        sge::free(_tag_buffer);
    }

    TagBuffer::TagBuffer(TagBuffer&& move)
        : _component_type(move._component_type),
        _ent_range(std::move(move._ent_range)),
        _tag_indices(std::move(move._tag_indices)),
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

    const TypeInfo* TagBuffer::component_type() const
    {
        return _component_type;
    }

    const EntityId* TagBuffer::ent_range() const
    {
        return _ent_range.data();
    }

    std::size_t TagBuffer::ent_range_len() const
    {
        return _ent_range.size();
    }

    const TagIndex_t* TagBuffer::tag_indices() const
    {
        return _tag_indices.data();
    }

    const TagCount_t* TagBuffer::tag_counts() const
    {
        if (_tag_counts.empty())
        {
            return nullptr;
        }

        return _tag_counts.data();
    }

    const void* TagBuffer::tag_buffer() const
    {
        return _tag_buffer;
    }

    std::size_t TagBuffer::tag_buffer_size() const
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
