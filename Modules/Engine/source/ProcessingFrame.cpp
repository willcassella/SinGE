// ProcessingFrame.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/ProcessingFrame.h"

namespace sge
{
	struct TagHeader
	{
		//////////////////
		///   Fields   ///
	public:

		ComponentId component;
		const TypeInfo* tag_type;
	};

	ProcessingFrame::ProcessingFrame(const Scene& /*unused*/)
		: _free_offset(0)
	{
	}

	ProcessingFrame::~ProcessingFrame()
	{
		// Destroy all tag objects
		const std::size_t freeOffset = _free_offset;
		for (std::size_t offset = 0; offset < freeOffset;)
		{
			// Access the header and tag
			auto* header = reinterpret_cast<TagHeader*>(&_tag_buffer[offset]);
			void* tag = &_tag_buffer[offset + sizeof(TagHeader)];

			// Destroy the tag object
			header->tag_type->drop(tag);

			// Move the offset forward
			offset += sizeof(TagHeader) + header->tag_type->size();
		}
	}

	void* ProcessingFrame::create_tag(ComponentId component, const TypeInfo& tagType)
	{
		// Acquire an offset into the buffer
		auto bufferOffset = _free_offset;
		_free_offset += sizeof(TagHeader) + tagType.size();

		// Make sure that we haven't run out of room in the tag buffer
		assert(_free_offset < TAG_BUFFER_SIZE /*Tag buffer overflow*/);

		// Insert the tag header
		new (&_tag_buffer[bufferOffset]) TagHeader{ component, &tagType };

		// Insert the tag object
		void* tag = &_tag_buffer[bufferOffset + tagType.size()];
		tagType.init(tag);
		return tag;
	}

	ProcessingFrameMut::ProcessingFrameMut(Scene& scene)
		: ProcessingFrame(scene),
		_scene(&scene)
	{
		_component_interface_free_offset = 0;
		_new_component_next = 0;
	}
}
