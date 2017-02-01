// ProcessingFrame.h
#pragma once

#include "Component.h"
#include "TagBuffer.h"

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

        std::size_t iteration_index() const;

        //////////////////
        ///   Fields   ///
	private:

        EntityId _entity;
        std::size_t _frame_index;
        std::size_t _iteration_index;
	};
}
