// ProcessingFrame.cpp

#include <algorithm>
#include <Core/Memory/Functions.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/ProcessingFrame.h"

SGE_REFLECT_TYPE(sge::ProcessingFrame);

namespace sge
{
	ProcessingFrame::ProcessingFrame()
        : _entity(NULL_ENTITY),
        _frame_index(0),
        _iteration_index(0)
	{
	}

    EntityId ProcessingFrame::entity() const
    {
        return _entity;
    }

    std::size_t ProcessingFrame::frame_index() const
    {
        return _frame_index;
    }

    std::size_t ProcessingFrame::iteration_index() const
    {
        return _iteration_index;
    }
}
