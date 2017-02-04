// ProcessingFrame.h
#pragma once

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
	private:

		ProcessingFrame();
		ProcessingFrame(const ProcessingFrame& copy) = delete;
	    ProcessingFrame& operator=(const ProcessingFrame& copy) = delete;
        ProcessingFrame(ProcessingFrame&& move) = delete;
	    ProcessingFrame& operator=(ProcessingFrame&& move) = delete;

        ///////////////////
        ///   Methods   ///
	public:

        EntityId entity() const;

        std::size_t frame_index() const;

        const EntityId* const* user_iterators() const;

        const EntityId* const* user_start_iterators() const;

        const EntityId* const* user_end_iterators() const;

        std::size_t num_user_iterators() const;

        std::size_t user_iterator_index(std::size_t iter_index) const;

        //////////////////
        ///   Fields   ///
	private:

        EntityId _entity;
        std::size_t _frame_index;
        const EntityId* const* _user_iterators;
        const EntityId* const* _user_start_iterators;
        const EntityId* const* _user_end_iterators;
        std::size_t _num_user_iterators;
	};
}
