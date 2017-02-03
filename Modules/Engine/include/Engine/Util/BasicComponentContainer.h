// BasicComponentContainer.h
#pragma once

#include <algorithm>
#include <Core/Interfaces/IFromString.h>
#include "../Component.h"
#include "../Util/VectorUtils.h"

namespace sge
{
    template <class ComponentT, typename ComponentDataT>
    class BasicComponentContainer final : public ComponentContainer
    {
        ///////////////////
        ///   Methods   ///
    public:

        void reset() override
        {
            _instance_set.clear();
            _instance_data.clear();
        }

        void to_archive(ArchiveWriter& writer) const override
        {
            for (std::size_t i = 0; i < _instance_set.size(); ++i)
            {
                writer.object_member(sge::to_string(_instance_set[i]).c_str(), _instance_data[i]);
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
                _instance_set.push_back(entity);

                // Add the data to the data set
                ComponentDataT data;
                sge::from_archive(data, reader);
                _instance_data.push_back(std::move(data));
            });

            // Make sure the data is ordered properly
            std::size_t num_dups = 0;
            const bool ordered = is_ordered(_instance_set.data(), _instance_set.size(), num_dups);
            if (!ordered || num_dups != 0)
            {
                assert(false);
                reset();
            }
        }

        InstanceIterator get_start_iterator() const override
        {
            return _instance_set.data();
        }

        InstanceIterator get_end_iterator() const override
        {
            return _instance_set.data() + _instance_set.size();
        }

        void create_instances(const EntityId* ordered_instances, std::size_t num, std::size_t num_dups) override
        {
            compact_merge_ord_entities(_instance_set, _instance_data, ordered_instances, num, num_dups);
        }

        void remove_instances(const EntityId* ordered_instances, std::size_t num) override
        {
            erase_ord_entities(_instance_set, _instance_data, ordered_instances, num);
        }

        void reset_interface(InstanceIterator instance, ComponentInterface* interf) override
        {
            auto& data = _instance_data[instance - _instance_set.data()];
            static_cast<ComponentT*>(interf)->reset(data);
        }

        //////////////////
        ///   Fields   ///
    private:

        std::vector<EntityId> _instance_set;
        std::vector<ComponentDataT> _instance_data;
    };
}
