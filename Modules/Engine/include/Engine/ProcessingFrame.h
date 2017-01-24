// ProcessingFrame.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "Component.h"

namespace sge
{
	struct SystemFrame;
    struct UpdatePipeline;

	struct SGE_ENGINE_API ProcessingFrame
	{
		SGE_REFLECTED_TYPE;

		/* Only 'SystemFrame' objects may construct ProcessingFrames. */
		friend SystemFrame;

		////////////////////////
		///   Constructors   ///
	public:

	    ProcessingFrame(ProcessingFrame&& move);
		~ProcessingFrame();

	private:

		ProcessingFrame();
		ProcessingFrame(const ProcessingFrame& copy) = delete;
		ProcessingFrame& operator=(const ProcessingFrame& copy) = delete;
        ProcessingFrame& operator=(ProcessingFrame&& move) = delete;

		///////////////////
		///   Methods   ///
	public:

        bool has_tags() const;

		/**
		 * \brief Creates a new tag for the given component.
		 * \param component The component to create the tag for.
		 * \param tagType The type of tag being created.
		 * \param tag The value of the tag object, to move from.
		 */
		void create_tag(ComponentId component, const TypeInfo& tagType, void* tag);

		/**
		 * \brief Creates a new tag for the given component.
		 * \tparam T The type of tag to create.
		 * \param component The component to create the tag for.
		 * \param tag The value of the tag object, to move from.
		 */
		template <typename T>
		void create_tag(ComponentId component, T&& tag)
		{
			this->create_tag(component, sge::get_type<T>(), &tag);
		}

		/**
		 * \brief Reserves additional space in the tag buffer for the given number of tags of the given size.
		 * \param num_tags The number of tag objects to reserve space for.
		 * \param tag_size The size of each tag object.
		 */
		void tag_buffer_reserve(std::size_t num_tags, std::size_t tag_size);

	private:

		/**
		 * \brief Expands the tag buffer by the given amount.
		 * \param additional_size The size to increase the tag buffer by.
		 */
		void tag_buffer_expand(std::size_t additional_size);

        void dispatch_tags(UpdatePipeline& pipeline, SystemFrame& callback_frame);

		//////////////////
		///   Fields   ///
	private:

		byte* _tag_buffer;
		std::size_t _buffer_size;
		std::size_t _free_offset;
	};
}
