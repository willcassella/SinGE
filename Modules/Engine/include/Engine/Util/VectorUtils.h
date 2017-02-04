// VectorUtils.h
#pragma once

#include "../Component.h"

namespace sge
{
    std::size_t SGE_ENGINE_API insert_ord_entities(
        std::vector<EntityId>& vec,
        const EntityId* ordered_entities,
        std::size_t num);

    void SGE_ENGINE_API remove_ord_entities(
        std::vector<EntityId>& vec,
        const EntityId* ordered_entities,
        std::size_t num);

    std::size_t SGE_ENGINE_API compact_ord_entities(
        EntityId* ordered_entities,
        std::size_t num);

    bool SGE_ENGINE_API is_ordered(
        const EntityId* ordered_entities,
        std::size_t num,
        std::size_t& out_num_dups);

    EntityId SGE_ENGINE_API ord_entity_union(
        const EntityId** SGE_RESTRICT iters,
        const EntityId* const* SGE_RESTRICT iter_lens,
        std::size_t num_iters);

    template <typename ContentContainerT>
    void compact_merge_ord_entities(
        std::vector<EntityId>& target_ord_instances,
        ContentContainerT& content_container,
        const EntityId* ord_new_instances,
        std::size_t num_new_instances,
        std::size_t min_est_dups)
    {
        target_ord_instances.insert(target_ord_instances.end(), num_new_instances - min_est_dups, NULL_ENTITY);
        content_container.insert(content_container.end(), num_new_instances - min_est_dups, {});
        auto target_back_iter = target_ord_instances.rbegin();
        auto content_back_iter = content_container.rbegin();
        std::size_t i = num_new_instances;

        // Start at the back, and iterate forwards
        auto target_front_iter = target_ord_instances.rbegin() + num_new_instances - min_est_dups;
        auto content_front_iter = content_container.rbegin() + num_new_instances - min_est_dups;

        EntityId last_new_instance = 0;
        for (const auto target_rend = target_ord_instances.rend(); i > 0 && target_front_iter != target_rend;)
        {
            // Get the new instance
            EntityId new_instance = ord_new_instances[i - 1];

            // If this is a duplicate
            if (new_instance == last_new_instance)
            {
                // Go to the next one
                --i;
                continue;
            }

            // If the new instance comes before the target
            if (*target_front_iter > new_instance)
            {
                // Put the existing one in
                *target_back_iter = *target_front_iter;
                *content_back_iter = std::move(*content_front_iter);
                ++target_front_iter;
                ++content_front_iter;
                ++target_back_iter;
                ++content_back_iter;
                continue;
            }

            last_new_instance = new_instance;

            // If the new instance comes after the target
            if (*target_front_iter < new_instance)
            {
                // Put new one in
                *target_back_iter = new_instance;
                --i;
                ++target_back_iter;
                ++content_back_iter;
                continue;
            }

            // We've hit a duplicate between new and target
            // Put the target in
            *target_back_iter = new_instance;
            *content_back_iter = std::move(*content_front_iter);
            --i;
            ++target_front_iter;
            ++content_front_iter;
            ++target_back_iter;
            ++content_back_iter;
        }

        // Put in all of the elements that were lower than the target front
        for (; i > 0; --i, ++target_back_iter, ++content_back_iter)
        {
            *target_back_iter = ord_new_instances[i - 1];
            *content_back_iter = {};
        }

        // If we didn't write up to the end of the array, there were duplicates
        if (target_back_iter != target_ord_instances.rend())
        {
            // Move everything back
            for (const auto target_rbegin = target_ord_instances.rbegin(); target_back_iter != target_rbegin;)
            {
                // Back up back and front iterators by one
                --target_front_iter;
                --content_front_iter;
                --target_back_iter;
                --content_back_iter;

                // Assign front to back
                *target_front_iter = *target_back_iter;
                *content_front_iter = std::move(*content_back_iter);
            }

            // Erase extra elements
            target_ord_instances.erase(target_front_iter.base(), target_ord_instances.end());
            content_container.erase(content_front_iter.base(), content_container.end());
        }
    }

    template <typename ContentContainerT>
    void erase_ord_entities(
        std::vector<EntityId>& ord_target_instances,
        ContentContainerT& content_container,
        const EntityId* ord_del_instances,
        std::size_t num_del_instances)
    {
        auto target_front_iter = ord_target_instances.begin();
        const auto target_end = ord_target_instances.end();
        std::size_t i = 0;

        // Iterate until we find the first instance that needs to be deleted
        for (; target_front_iter != target_end && i < num_del_instances;)
        {
            if (*target_front_iter < ord_del_instances[i])
            {
                ++target_front_iter;
                continue;
            }
            if (*target_front_iter > ord_del_instances[i])
            {
                ++i;
                continue;
            }

            break;
        }

        // Make sure we didn't bail out because there was nothing to delete
        if (target_front_iter == target_end || i >= num_del_instances)
        {
            return;
        }

        // Create back iterators, and advance front iterators by 1
        auto content_back_iter = content_container.begin() + i;
        auto content_front_iter = content_back_iter + 1;
        auto target_back_iter = target_front_iter;
        target_front_iter += 1;
        i += 1;

        // Start erasing
        for (; target_front_iter != target_end && i < num_del_instances;)
        {
            // If the current needs to be deleted
            if (*target_front_iter == ord_del_instances[i])
            {
                // Move the front forward
                ++i;
                ++target_front_iter;
                ++content_front_iter;
                continue;
            }

            if (*target_front_iter > ord_del_instances[i])
            {
                ++i;
                continue;
            }

            // Back up the current
            *target_back_iter = *target_front_iter;
            *content_back_iter = std::move(*content_front_iter);
            ++target_front_iter;
            ++content_front_iter;
            ++target_back_iter;
            ++content_back_iter;
        }

        // Back up remaining
        for (; target_front_iter != target_end; ++target_front_iter, ++target_back_iter, ++content_front_iter, ++content_back_iter)
        {
            *target_back_iter = *target_front_iter;
            *content_back_iter = std::move(*content_front_iter);
        }

        // Erase the extras
        ord_target_instances.erase(target_back_iter, target_end);
        content_container.erase(content_back_iter, content_container.end());
    }
}
