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
	ProcessingFrame::ProcessingFrame()
	{
	}

    ProcessingFrame::ProcessingFrame(ProcessingFrame&& move)
    {
    }

	ProcessingFrame::~ProcessingFrame()
	{
	}
}
