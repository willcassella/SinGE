// VectorUtils.cpp

#include "../../include/Engine/Util/VectorUtils.h"

namespace sge
{
    std::size_t insert_ord_entities(
        std::vector<EntityId>& target_ord_instances,
        const EntityId* ord_new_instances,
        std::size_t num_new_instances)
    {
        target_ord_instances.insert(target_ord_instances.end(), num_new_instances, NULL_ENTITY);
        auto target_back_iter = target_ord_instances.rbegin();
        std::size_t i = num_new_instances;
        std::size_t num_dups = 0;

        auto target_front_iter = target_ord_instances.rbegin() + num_new_instances;
        for (const auto target_rend = target_ord_instances.rend(); i > 0 && target_front_iter != target_rend; ++target_back_iter)
        {
            EntityId new_instance = ord_new_instances[i - 1];

            // If the new instance is greater
            if (*target_front_iter < new_instance)
            {
                // Put it in
                *target_back_iter = new_instance;
                --i;
                continue;
            }

            // If they're equal
            if (*target_front_iter == new_instance)
            {
                // Increment the dup count
                ++num_dups;
            }

            // Put the current one in
            *target_back_iter = *target_front_iter;
            ++target_front_iter;
        }

        for (; i > 0; --i, ++target_back_iter)
        {
            *target_back_iter = ord_new_instances[i - 1];
        }

        return num_dups;
    }

    void remove_ord_entities(
        std::vector<EntityId>& vec,
        const EntityId* ordered_entities,
        std::size_t num)
    {
        auto back_iter = vec.begin();
        auto front_iter = vec.begin();

        for (std::size_t i = 0; i < num; ++front_iter)
        {
            if (*front_iter == ordered_entities[i])
            {
                ++i;
                continue;
            }

            *back_iter = *front_iter;
            ++back_iter;
        }

        for (; front_iter != vec.end(); ++front_iter, ++back_iter)
        {
            *back_iter = *front_iter;
        }

        vec.erase(back_iter, vec.end());
    }

    std::size_t compact_ord_entities(
        EntityId* ordered_entities,
        std::size_t num)
    {
        EntityId* front = ordered_entities;
        EntityId* back = ordered_entities;
        EntityId last_entity = NULL_ENTITY;

        while (front != ordered_entities + num)
        {
            // If the one we're looking at is equal to the one we just looked at (or it's the null entity)
            if (*front == last_entity)
            {
                ++front;
                continue;
            }

            // Move forward
            last_entity = *front;
            *back = *front;
            ++front;
            ++back;
        }

        return back - ordered_entities;
    }

    bool is_ordered(
        const EntityId* ordered_entities,
        std::size_t num,
        std::size_t& out_num_dups)
    {
        out_num_dups = 0;
        EntityId last_entity = WORLD_ENTITY;

        for (std::size_t i = 0; i < num; ++i)
        {
            const auto entity = ordered_entities[i];

            // If the current one is less than the last
            if (entity < last_entity)
            {
                return false;
            }

            if (entity == last_entity)
            {
                ++out_num_dups;
            }

            last_entity = entity;
        }

        return true;
    }
}
