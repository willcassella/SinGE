// ProcessingFrame.cpp

#include <algorithm>
#include <Core/Memory/Functions.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/ProcessingFrame.h"
#include "../include/Engine/Scene.h"
#include "../include/Engine/UpdatePipeline.h"

SGE_REFLECT_TYPE(sge::ProcessingFrame);

namespace sge
{
	struct SGE_ALIGNED_BUFFER_HEADER TagHeader
	{
		//////////////////
		///   Fields   ///
	public:

		ComponentId component;
		const TypeInfo* tag_type;
	};

	ProcessingFrame::ProcessingFrame()
		: _tag_buffer(nullptr),
		_buffer_size(0),
		_free_offset(0)
	{
	}

    ProcessingFrame::ProcessingFrame(ProcessingFrame&& move)
        : _tag_buffer(move._tag_buffer),
        _buffer_size(move._buffer_size),
        _free_offset(move._free_offset)
    {
        move._tag_buffer = nullptr;
        move._buffer_size = 0;
        move._free_offset = 0;
    }

	ProcessingFrame::~ProcessingFrame()
	{
		// Destroy all tag objects
		const std::size_t freeOffset = _free_offset;
		for (std::size_t offset = 0; offset < freeOffset;)
		{
			// Access the header and tag
			auto* header = reinterpret_cast<TagHeader*>(_tag_buffer + offset);
			void* tag = _tag_buffer + offset + sizeof(TagHeader);

			// Destroy the tag object
			header->tag_type->drop(tag);

			// Destroy the tag header (no-op, but for good measure)
			header->~TagHeader();

			// Move the offset forward
			offset += sizeof(TagHeader) + header->tag_type->aligned_size();
		}

		// Free the tag buffer
		sge::free(_tag_buffer);
	}

    bool ProcessingFrame::has_tags() const
    {
        return _free_offset != 0;
    }

	void ProcessingFrame::create_tag(ComponentId component, const TypeInfo& tagType, void* tag)
	{
		const auto aligned_size = tagType.aligned_size(alignof(std::max_align_t));

		// Reserve space for the tag
		tag_buffer_reserve(1, aligned_size);

		// Acquire an offset into the buffer
		auto bufferOffset = _free_offset;
		_free_offset += sizeof(TagHeader) + aligned_size;

		// Insert the tag header
		new (_tag_buffer + bufferOffset) TagHeader{ component, &tagType };

		// Insert the tag object
		tagType.move_init(_tag_buffer + bufferOffset + sizeof(TagHeader), tag);
	}

	void ProcessingFrame::tag_buffer_reserve(std::size_t num_tags, std::size_t tag_size)
	{
		const auto requested = (tag_size + sizeof(TagHeader)) * num_tags;

		if (_buffer_size == 0)
		{
			_tag_buffer = (byte*)sge::malloc(requested);
			_buffer_size = requested;
		}
		else if (_buffer_size - _free_offset < requested)
		{
			tag_buffer_expand(requested - (_buffer_size - _free_offset));
		}
	}

	void ProcessingFrame::tag_buffer_expand(std::size_t additional_size)
	{
		// Allocate a new buffer
		byte* buff = (byte*)sge::malloc(_buffer_size + additional_size);

		// Move everything into the new buffer
		for (std::size_t offset = 0; offset < _free_offset;)
		{
			// Get the current header
			auto* header = reinterpret_cast<TagHeader*>(_tag_buffer + offset);

			// Copy it to the target
			auto* new_header = new (buff + offset) TagHeader{ *header };

			// Move the value
			header->tag_type->move_init(new_header + 1, header + 1);

			// Destroy the original
			header->tag_type->drop(header + 1);

			// Increment the offset
			offset += sizeof(TagHeader) + header->tag_type->aligned_size();
		}

		// Free the old buffer
		sge::free(_tag_buffer);

		// Replace it
		_tag_buffer = buff;
		_buffer_size += additional_size;
	}

    void ProcessingFrame::dispatch_tags(UpdatePipeline& pipeline, SystemFrame& callback_frame)
    {
        // Destroy all tag objects
        const std::size_t freeOffset = _free_offset;
        for (std::size_t offset = 0; offset < freeOffset;)
        {
            // Access the header and tag
            auto* header = reinterpret_cast<TagHeader*>(_tag_buffer + offset);
            void* tag = _tag_buffer + offset + sizeof(TagHeader);

            // Run the tag callback
            pipeline.run_tag(Any<>{ *header->tag_type, tag }, header->component, callback_frame);

            // Move the offset forward
            offset += sizeof(TagHeader) + header->tag_type->aligned_size();
        }
    }
}
