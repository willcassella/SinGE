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
        _user_iterators(nullptr),
        _user_start_iterators(nullptr),
        _user_end_iterators(nullptr),
        _num_user_iterators(0)
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

    const EntityId* const* ProcessingFrame::user_iterators() const
    {
        return _user_iterators;
    }

    const EntityId* const* ProcessingFrame::user_start_iterators() const
    {
        return _user_start_iterators;
    }

    const EntityId* const* ProcessingFrame::user_end_iterators() const
    {
        return _user_end_iterators;
    }

    std::size_t ProcessingFrame::num_user_iterators() const
    {
        return _num_user_iterators;
    }

    std::size_t ProcessingFrame::user_iterator_index(std::size_t iter_index) const
    {
        return user_iterators()[iter_index] - user_start_iterators()[iter_index];
    }
}
