// MapComponentContainer.h
#pragma once

#include <map>
#include <Core/Interfaces/IFromString.h>
#include "../Component.h"
#include "VectorUtils.h"

namespace sge
{
    template <class ComponentT, typename ComponentDataT>
    class MapComponentContainer final : public ComponentContainer
    {
        ///////////////////
        ///   Methods   ///
    public:

        void reset() override
        {
            instance_set.clear();
            instance_data.clear();
        }

        void to_archive(ArchiveWriter& writer) const override
        {
            for (const auto& element : instance_data)
            {
                writer.object_member(sge::to_string(element.first).c_str(), element.second);
            }
        }

        void from_archive(ArchiveReader& reader) override
        {
            reset();

            reader.enumerate_object_members([this, &reader](const char* entity_id_str)
            {
                // Get the entity id
                EntityId entity = NULL_ENTITY;
                sge::from_string(entity, entity_id_str, std::strlen(entity_id_str));

                // Make sure it's valid
                if (entity == NULL_ENTITY)
                {
                    return;
                }

                // Add the entity to the instance set
                instance_set.push_back(entity);

                // Add the data to the data set
                ComponentDataT data;
                sge::from_archive(data, reader);
                instance_data[entity] = std::move(data);
            });

            // Make sure the data is ordered properly
            std::size_t num_dups = 0;
            const bool ordered = is_ordered(instance_set.data(), instance_set.size(), num_dups);
            if (!ordered || num_dups != 0)
            {
                assert(false);
                reset();
            }
        }

        InstanceIterator get_start_iterator() const override
        {
            return instance_set.data();
        }

        InstanceIterator get_end_iterator() const override
        {
            return instance_set.data() + instance_set.size();
        }

        void create_instances(const EntityId* ordered_instances, std::size_t num, std::size_t num_dups) override
        {
            insert_ord_entities(instance_set, ordered_instances, num);
            const auto len = compact_ord_entities(instance_set.data(), instance_set.size());
            instance_set.erase(instance_set.begin() + len, instance_set.end());

            for (std::size_t i = 0; i < num; ++i)
            {
                instance_data.insert(std::make_pair(ordered_instances[i], ComponentDataT{}));
            }
        }

        void remove_instances(const EntityId* ordered_instances, std::size_t num) override
        {
            remove_ord_entities(instance_set, ordered_instances, num);

            for (std::size_t i = 0; i < num; ++i)
            {
                instance_data.erase(ordered_instances[i]);
            }
        }

        void reset_interface(InstanceIterator instance, ComponentInterface* interf) override
        {
            auto& data = instance_data[*instance];
            static_cast<ComponentT*>(interf)->reset(data, instance_data);
        }

        //////////////////
        ///   Fields   ///
    public:

        std::vector<EntityId> instance_set;
        std::map<EntityId, ComponentDataT> instance_data;
    };
}
