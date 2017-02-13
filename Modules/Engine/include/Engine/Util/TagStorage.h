// TagStorage.h
#pragma once

#include "../TagBuffer.h"

namespace sge
{
    template <typename TagT>
    struct TagStorage
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        TagStorage()
        {
        }

        ///////////////////
        ///   Methods   ///
    public:

        void add_tag(EntityId entity, TagT value)
        {
            if (!_ent_range.empty() && _ent_range.back() == entity)
            {
                _tag_counts.back() += 1;
            }
            else
            {
                _tag_indices.push_back(static_cast<TagIndex_t>(_ent_range.size()));
                _tag_counts.push_back(1);
                _ent_range.push_back(entity);
            }

            if (std::is_empty<TagT>::value)
            {
                return;
            }

            _tags.push_back(std::move(value));
        }

        void add_single_tag(EntityId entity, TagT value)
        {
            if (!_ent_range.empty() && _ent_range.back() == entity)
            {
                return;
            }

            _ent_range.push_back(entity);

            if (std::is_empty<TagT>::value)
            {
                return;
            }

            _tags.push_back(std::move(value));
        }

        void create_buffer(const TypeInfo* component_type, std::map<const TypeInfo*, std::vector<TagBuffer>>& tag_map) const
        {
            // Don't add anything to the map if there's nothing to add
            if (_ent_range.size() == 0)
            {
                return;
            }

            // If the tags are empty
            if (std::is_empty<TagT>::value)
            {
                // If the tags are single
                if (_tag_counts.empty())
                {
                    tag_map[&sge::get_type<TagT>()].push_back(TagBuffer::create_single_empty(
                        component_type,
                        _ent_range.data(),
                        _ent_range.size()));
                    return;
                }

                tag_map[&sge::get_type<TagT>()].push_back(TagBuffer::create_empty(
                    component_type,
                    _ent_range.data(),
                    _tag_counts.data(),
                    _ent_range.size()));
                return;
            }

            // If the tags are single
            if (_tag_counts.empty())
            {
                tag_map[&sge::get_type<TagT>()].push_back(TagBuffer::create_single(
                    component_type,
                    _ent_range.data(),
                    _ent_range.size(),
                    _tags.data(),
                    _tags.size() * sizeof(TagT)));
                return;
            }

            tag_map[&sge::get_type<TagT>()].push_back(TagBuffer::create(
                component_type,
                _ent_range.data(),
                _tag_indices.data(),
                _tag_counts.data(),
                _ent_range.size(),
                _tags.data(),
                _tags.size() * sizeof(TagT)));
        }

        //////////////////
        ///   Fields   ///
    private:

        std::vector<EntityId> _ent_range;
        std::vector<TagIndex_t> _tag_indices;
        std::vector<TagCount_t> _tag_counts;
        std::vector<TagT> _tags;
    };
}
