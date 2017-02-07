// EntityRangePack.h
#pragma once

#include "../TagBuffer.h"

namespace sge
{
    template <typename ... RangeTs>
    struct EntityRangePack
    {
        static constexpr std::size_t NUM_RANGES = sizeof...(RangeTs);

        using RangeArray = std::array<const EntityId*, NUM_RANGES>;

        using LengthArray = std::array<std::size_t, NUM_RANGES>;

        template <std::size_t I>
        using IteratorIndex = std::integral_constant<std::size_t, I>;

        ///////////////////
        ///   Methods   ///
    public:

        RangeArray get_entity_ranges() const
        {
            RangeArray result;
            return this->get_entity_ranges(IteratorIndex<0>{}, result);
        }

        LengthArray get_range_lengths() const
        {
            LengthArray result;
            return this->get_range_lengths(IteratorIndex<0>{}, result);
        }

    private:

        template <std::size_t I>
        RangeArray get_entity_ranges(IteratorIndex<I>, RangeArray& array) const
        {
            array[I] = std::get<I>(ranges).get_start();
            return this->get_entity_ranges(IteratorIndex<I + 1>{}, array);
        }

        RangeArray get_entity_ranges(IteratorIndex<NUM_RANGES>, RangeArray& array) const
        {
            return array;
        }

        template <std::size_t I>
        LengthArray get_range_lengths(IteratorIndex<I>, LengthArray& array) const
        {
            array[I] = std::get<I>(ranges).get_length();
            return this->get_range_lengths(IteratorIndex<I + 1>{}, array);
        }

        LengthArray get_range_lengths(IteratorIndex<NUM_RANGES>, LengthArray& array) const
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
        result.ranges = std::make_tuple(std::move(ranges)...);
        return result;
    }

    struct OrdEntityRange
    {
        ///////////////////
        ///   Methods   ///
    public:

        const EntityId* get_start() const
        {
            return ord_entities;
        }

        std::size_t get_length() const
        {
            return size;
        }

        //////////////////
        ///   Fields   ///
    public:

        const EntityId* ord_entities;
        std::size_t size;
    };

    inline OrdEntityRange ord_ents_range(const EntityId* ord_entities, std::size_t num)
    {
        return OrdEntityRange{ ord_entities, num };
    }

    inline OrdEntityRange ord_ents_range(const EntityId* ord_entities_start, const EntityId* ord_entities_end)
    {
        return OrdEntityRange{ ord_entities_start, static_cast<std::size_t>(ord_entities_end - ord_entities_start) };
    }

    inline OrdEntityRange ord_ents_range(const std::vector<EntityId>& ord_entities)
    {
        return OrdEntityRange{ ord_entities.data(), ord_entities.size() };
    }

    inline EntityRangePack<OrdEntityRange> zip_ord_ents(const EntityId* ord_entities, std::size_t num)
    {
        return zip(OrdEntityRange{ ord_entities, num });
    }
}
