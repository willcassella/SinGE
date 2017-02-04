// EntityRangePack.h
#pragma once

#include "../TagBuffer.h"

namespace sge
{
    template <typename ... RangeTs>
    struct EntityRangePack
    {
        using IteratorArray = std::array<const EntityId*, sizeof...(RangeTs)>;

        template <std::size_t I>
        using IteratorIndex = std::integral_constant<std::size_t, I>;

        static constexpr std::size_t NUM_ITERATORS = sizeof...(RangeTs);

        ///////////////////
        ///   Methods   ///
    public:

        IteratorArray get_start_iterators() const
        {
            IteratorArray result;
            return get_start_iterators(IteratorIndex<0>{}, result);
        }

        IteratorArray get_end_iterators() const
        {
            IteratorArray result;
            return get_end_iterators(IteratorIndex<0>{}, result);
        }

    private:

        template <std::size_t I>
        IteratorArray get_start_iterators(IteratorIndex<I>, IteratorArray& array) const
        {
            using std::begin;
            array[I] = begin(std::get<I>(iterators));
            return get_start_iterators(IteratorIndex<I + 1>{}, array);
        }

        IteratorArray get_start_iterators(IteratorIndex<NUM_ITERATORS>, IteratorArray& array) const
        {
            return array;
        }

        template <std::size_t I>
        IteratorArray get_end_iterators(IteratorIndex<I>, IteratorArray& array) const
        {
            using std::end;
            array[I] = end(std::get<I>(iterators));
            return get_end_iterators(IteratorIndex<I + 1>{}, array);
        }

        IteratorArray get_end_iterators(IteratorIndex<NUM_ITERATORS>, IteratorArray& array) const
        {
            return array;
        }

        //////////////////
        ///   Fields   ///
    public:

        std::tuple<RangeTs...> ranges;
    };

    template <typename ... RangeTs>
    EntityRangePack<RangeTs...> zip(RangeTs... ranges)
    {
        EntityRangePack<RangeTs...> result;
        result.ranges = std::make_tuple(std::move(ranges...));
        return result;
    }

    struct OrdEntityRange
    {
        ///////////////////
        ///   Methods   ///
    public:

        const EntityId* begin() const
        {
            return ord_entities;
        }

        const EntityId* end() const
        {
            return ord_entities + size;
        }

        //////////////////
        ///   Fields   ///
    public:

        const EntityId* ord_entities;
        std::size_t size;
    };

    inline EntityRangePack<OrdEntityRange> zip_ord_ents(const EntityId* ord_entities, std::size_t num)
    {
        return zip(OrdEntityRange{ ord_entities, num });
    }
}
