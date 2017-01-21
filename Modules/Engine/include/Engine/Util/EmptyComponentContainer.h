// EmptyComponentContainer.h
#pragma once

#include "../Component.h"

namespace sge
{
    template <typename ComponentT>
    class EmptyComponentContainer final : public ComponentContainer
    {
        void reset() override
        {
        }

        void to_archive(ArchiveWriter& writer, const std::set<EntityId>& est_instances) const override
        {
            for (auto instance : est_instances)
            {
                writer.array_element(instance);
            }
        }

        void from_archive(ArchiveReader& reader, std::set<EntityId>& est_instances) override
        {
            reader.enumerate_array_elements([&reader, &est_instances](std::size_t /*i*/)
            {
                EntityId entity = NULL_ENTITY;
                sge::from_archive(entity, reader);

                if (entity != NULL_ENTITY)
                {
                    est_instances.insert(entity);
                }
            });
        }

        void create_component(EntityId /*entity*/) override
        {
        }

        void remove_component(EntityId /*entity*/) override
        {
        }

        bool create_interface(ProcessingFrame& pframe, EntityId entity, void* addr) override
        {
            new (addr) ComponentT{ pframe, entity };
            return true;
        }
    };
}
