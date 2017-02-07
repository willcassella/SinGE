// EmptyComponentContainer.h
#pragma once

#include <algorithm>
#include "../Component.h"
#include "../Util/VectorUtils.h"

namespace sge
{
    template <typename ComponentT>
    class EmptyComponentContainer final : public ComponentContainer
    {
        void reset() override
        {
            _instance_set.clear();
        }

        void to_archive(ArchiveWriter& writer) const override
        {
            writer.typed_array(_instance_set.data(), _instance_set.size());
        }

        void from_archive(ArchiveReader& reader) override
        {
            reset();
            std::size_t size = 0;
            if (!reader.array_size(size))
            {
                return;
            }

            // Fill the instance set with the components from the archive
            _instance_set.assign(size, 0);
            reader.typed_array(_instance_set.data(), _instance_set.size());

            // Sort them
            std::sort(_instance_set.begin(), _instance_set.end());

            // Remove duplicates
            size = compact_ord_entities(_instance_set.data(), _instance_set.size());
            _instance_set.erase(_instance_set.begin() + size, _instance_set.end());
        }

        const EntityId* get_instance_range() const override
        {
            return _instance_set.data();
        }

        std::size_t get_instance_range_length() const override
        {
            return _instance_set.size();
        }

        void create_instances(
            const EntityId* const* ordered_entities,
            const std::size_t* lens,
            std::size_t num_arrays,
            std::size_t num_new_entities,
            EntityId* out_new_instances) override
        {
            const std::size_t old_len = _instance_set.size();
            _instance_set.insert(_instance_set.end(), num_new_entities, NULL_ENTITY);

            rev_multi_insert(_instance_set.data(), old_len, _instance_set.size(), ordered_entities, lens, num_arrays,
                [&](std::size_t new_index, std::size_t old_index) // Swap function
            {
                _instance_set[new_index] = _instance_set[old_index];
            },
                [&](EntityId new_entity, std::size_t index) // Insert function
            {
                _instance_set[index] = new_entity;
                out_new_instances[num_new_entities - 1] = new_entity;
                --num_new_entities;
            });
        }

        void remove_instances(const EntityId* ordered_entities, std::size_t num) override
        {
            remove_ord_entities(_instance_set, ordered_entities, num);
        }

        void reset_interface(std::size_t /*instance_index*/, ComponentInterface* interf) override
        {
            static_cast<ComponentT*>(interf)->reset();
        }

        //////////////////
        ///   Fields   ///
    private:

        std::vector<EntityId> _instance_set;
    };
}
