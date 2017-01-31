// ProcessingFrame.h
#pragma once

#include "Component.h"
#include "TagBuffer.h"

namespace sge
{
	struct SystemFrame;
    struct UpdatePipeline;

    // TODO: Make this useful again (I have a few ideas)
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
	};
}
