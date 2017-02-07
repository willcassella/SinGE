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
        _num_user_ranges(0),
        _user_ranges(nullptr),
        _user_range_lens(nullptr),
        _user_iterators(nullptr)
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

    std::size_t ProcessingFrame::num_user_ranges() const
    {
        return _num_user_ranges;
    }

    const EntityId* const* ProcessingFrame::user_entity_ranges() const
    {
        return _user_ranges;
    }

    const std::size_t* ProcessingFrame::user_entity_range_lengths() const
    {
        return _user_range_lens;
    }

    const std::size_t* ProcessingFrame::user_iterators() const
    {
        return _user_iterators;
    }

    std::size_t ProcessingFrame::user_iterator(std::size_t iter_index) const
    {
        assert(iter_index < _num_user_ranges);
        return user_iterators()[iter_index];
    }
}
