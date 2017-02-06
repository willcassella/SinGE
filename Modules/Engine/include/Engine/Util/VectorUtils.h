// VectorUtils.h
#pragma once

#include <Core/Memory/Functions.h>
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
        const EntityId* const* SGE_RESTRICT end_iters,
        std::size_t num_iters);

    EntityId SGE_ENGINE_API ord_entities_match(
        const EntityId* const* ord_entity_arrays,
        const std::size_t* SGE_RESTRICT lens,
        std::size_t* SGE_RESTRICT iters,
        std::size_t required_len,
        std::size_t inv_required_len,
        std::size_t num_arrays,
        EntityId target = NULL_ENTITY);

    std::size_t SGE_ENGINE_API rev_count_dups(
        const EntityId* const* ord_ent_arrays,
        const std::size_t* lens,
        std::size_t num_arrays);

    template <typename InsertFn, typename SwapFn>
    void rev_multi_insert(
        const EntityId* target_ord_instances,
        std::size_t target_original_len,
        std::size_t target_new_len,
        const EntityId* const* ord_new_instances,
        const std::size_t* lens,
        std::size_t num_arrays,
        SwapFn&& swap_fn,
        InsertFn&& insert_fn)
    {
        std::size_t num_to_insert = target_new_len - target_original_len;

        // Start at the back, and iterate forwards
        auto target_rev_back_iter = target_new_len;
        auto target_rev_front_iter = target_original_len;

        // Create a buffer to hold the iterators
        auto* iters = SGE_STACK_ALLOC(std::size_t, num_arrays);
        std::memcpy(iters, lens, sizeof(std::size_t) * num_arrays);

        while (target_rev_front_iter != 0 && num_to_insert != 0)
        {
            EntityId new_instance = NULL_ENTITY;
            std::size_t new_instance_index = 0;
            const EntityId current_instance = target_ord_instances[target_rev_front_iter - 1];

            for (std::size_t i = 0; i < num_arrays; ++i)
            {
                // Make sure we haven't reached the end of this iterator
                if (iters[i] == 0)
                {
                    continue;
                }

                // Get the new instance for this iterator
                EntityId iter_new_instance = ord_new_instances[i][iters[i] - 1];

                // If this instance shouldn't go in yet
                if (iter_new_instance < new_instance || iter_new_instance < current_instance)
                {
                    continue;
                }

                // If this is a duplicate
                if (iter_new_instance == new_instance || iter_new_instance == current_instance)
                {
                    --iters[i];
                    continue;
                }

                // Otherwise, make this the new instance
                new_instance = iter_new_instance;
                new_instance_index = i;
            }

            // If we found a candidate to insert
            if (new_instance == NULL_ENTITY)
            {
                // Call the user's swap function
                swap_fn(target_rev_back_iter - 1, target_rev_front_iter - 1);

                // Put the existing one in
                --target_rev_front_iter;
                --target_rev_back_iter;
                continue;
            }

            // The new instance comes after the target
            insert_fn(new_instance, target_rev_back_iter - 1);
            --iters[new_instance_index];
            --target_rev_back_iter;
            --num_to_insert;
        }

        // Put in all of the elements that were lower than the target front
        while (target_rev_back_iter != 0 && num_to_insert != 0)
        {
            EntityId new_instance = NULL_ENTITY;
            std::size_t new_instance_index = 0;

            for (std::size_t i = 0; i < num_arrays; ++i)
            {
                // Make sure we haven't reached the end of this iterator
                if (iters[i] == 0)
                {
                    continue;
                }

                // Get the new instance for this iterator
                EntityId iter_new_instance = ord_new_instances[i][iters[i] - 1];

                // If this instance shouldn't go in yet
                if (iter_new_instance < new_instance)
                {
                    continue;
                }

                // If this is a duplicate
                if (iter_new_instance == new_instance)
                {
                    --iters[i];
                    continue;
                }

                // Otherwise, make this the new instance
                new_instance = iter_new_instance;
                new_instance_index = i;
            }

            // Call the user's insert function
            insert_fn(new_instance, target_rev_back_iter - 1);
            --target_rev_back_iter;
            --num_to_insert;
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
