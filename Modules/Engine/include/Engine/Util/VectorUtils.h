// VectorUtils.h
#pragma once

#include "../Component.h"

namespace sge
{
    void SGE_ENGINE_API insert_ord_entities(
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
}
