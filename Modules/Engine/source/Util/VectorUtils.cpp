// VectorUtils.cpp

#include "../../include/Engine/Util/VectorUtils.h"

namespace sge
{
    void insert_ord_entities(
        std::vector<EntityId>& vec,
        const EntityId* ordered_entities,
        std::size_t num)
    {
        vec.insert(vec.end(), num, 0);
        auto rev_back_iter = vec.rbegin();
        std::size_t i = num;

        for (auto rev_front_iter = vec.rbegin() + num; i > 0 && rev_front_iter != vec.rend(); ++rev_back_iter)
        {
            // If the value in the array is greater than the current value
            if (*rev_front_iter < ordered_entities[i - 1])
            {
                // Put it in
                *rev_back_iter = ordered_entities[i - 1];
                --i;
                continue;
            }

            *rev_back_iter = *rev_front_iter;
            ++rev_front_iter;
        }

        for (; i > 0; --i, ++rev_back_iter)
        {
            *rev_back_iter = ordered_entities[i - 1];
        }
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
            if (*front == last_entity || *front == NULL_ENTITY)
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
}
