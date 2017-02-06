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

        const EntityId* get_instance_set() const override
        {
            return instance_set.data();
        }

        std::size_t get_num_instances() const override
        {
            return instance_set.size();
        }

        void create_instances(
            const EntityId* const* ordered_instances,
            const std::size_t* lens,
            std::size_t num_arrays,
            std::size_t num_new_instances,
            EntityId* out_new_instances) override
        {
            const std::size_t old_len = instance_set.size();
            instance_set.insert(instance_set.end(), num_new_instances, NULL_ENTITY);

            rev_multi_insert(instance_set.data(), old_len, instance_set.size(), ordered_instances, lens, num_arrays,
                [&](std::size_t new_index, std::size_t old_index) // Swap function
            {
                instance_set[new_index] = instance_set[old_index];
            },
                [&](EntityId new_entity, std::size_t index) // Insert function
            {
                instance_set[index] = new_entity;
                instance_data[new_entity] = ComponentDataT{};
                out_new_instances[num_new_instances - 1] = new_entity;
                --num_new_instances;
            });
        }

        void remove_instances(const EntityId* ordered_instances, std::size_t num) override
        {
            remove_ord_entities(instance_set, ordered_instances, num);

            for (std::size_t i = 0; i < num; ++i)
            {
                instance_data.erase(ordered_instances[i]);
            }
        }

        void reset_interface(std::size_t instance_index, ComponentInterface* interf) override
        {
            const auto entity = instance_set[instance_index];
            auto& data = instance_data[entity];
            static_cast<ComponentT*>(interf)->reset(data, instance_data);
        }

        //////////////////
        ///   Fields   ///
    public:

        std::vector<EntityId> instance_set;
        std::map<EntityId, ComponentDataT> instance_data;
    };
}
