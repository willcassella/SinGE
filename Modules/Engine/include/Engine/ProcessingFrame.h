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

        std::size_t num_user_ranges() const;

        const EntityId* const* user_entity_ranges() const;

        const std::size_t* user_entity_range_lengths() const;

        const std::size_t* user_iterators() const;

        std::size_t user_iterator(std::size_t iter_index) const;

        //////////////////
        ///   Fields   ///
    private:

        EntityId _entity;
        std::size_t _frame_index;
        std::size_t _num_user_ranges;
        const EntityId* const* _user_ranges;
        const std::size_t* _user_range_lens;
        const std::size_t* _user_iterators;
    };
}
