// TagBuffer.h
#pragma once

#include "Component.h"

namespace sge
{
    struct TypeInfo;

    using TagCount_t = uint16;
    using TagIndex_t = uint32;

    struct SGE_ENGINE_API TagBuffer
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        /**
        * \brief Creates a tag buffer where each component may have one or more instances of non-empty tags.
        * \param component_type The component type these tags are being created for.
        * \param ent_range The entities these tags are being created for.
        * \param tag_indices An array that specifies the offset into the tag buffer for the tags belonging to each entity.
        * \param tag_counts An array that specfies the length into the tag buffer for the tags belonging to each entity.
        * \param ent_range_len The number of entities in the array.
        * \param tag_buffer A buffer containing the tag values. Tags may be POD types only.
        * \param tag_buffer_size The size of the tag buffer.
        * \return An immutable tag buffer.
        */
        static TagBuffer create(
            const TypeInfo* component_type,
            const EntityId* ent_range,
            const TagIndex_t* tag_indices,
            const TagCount_t* tag_counts,
            std::size_t ent_range_len,
            const void* tag_buffer,
            std::size_t tag_buffer_size);

        /**
         * \brief Creates a tag buffer where each component has exactly one instance of a non-empty tag.
         * \param component_type The type of component this tag buffer is associated with.
         * \param ent_range The entities for the components the tags are being created for.
         * \param ent_range_len The number of entities in the array.
         * \param tag_buffer A buffer containing the tag values.
         * \param tag_buffer_size The size of the buffer containing the tags.
         * \return An immutable tag buffer.
         */
        static TagBuffer create_single(
            const TypeInfo* component_type,
            const EntityId* ent_range,
            std::size_t ent_range_len,
            const void* tag_buffer,
            std::size_t tag_buffer_size);

        /**
         * \brief Creates a tag buffer where each component may have one or more empty tags associated with it.
         * \param component_type The component type these tags are being created for.
         * \param ent_range The entities for the components the tags are being created for.
         * \param tag_counts A count of the number of tags given for each entity.
         * \param ent_range_len The number of entities.
         * \return An immutable tag buffer.
         */
        static TagBuffer create_empty(
            const TypeInfo* component_type,
            const EntityId* ent_range,
            const TagCount_t* tag_counts,
            std::size_t ent_range_len);

        /**
         * \brief Creates a tag buffer where each component has exactly one empty tag associated with it.
         * \param component_type The component type these tags are being created for.
         * \param ord_entities The entities for the components the tags are being applied to. This must be ordered and contain no duplicates.
         * \param num_ents A count of the number of tags given for each entity.
         * \return An immutable tag buffer.
         */
        static TagBuffer create_single_empty(
            const TypeInfo* component_type,
            const EntityId* ord_entities,
            std::size_t num_ents);

        ~TagBuffer();
        TagBuffer(const TagBuffer& copy) = delete;
        TagBuffer& operator=(const TagBuffer& copy) = delete;
        TagBuffer(TagBuffer&& move);
        TagBuffer& operator=(TagBuffer&& move) = delete;

    private:

        TagBuffer();

        ///////////////////
        ///   Methods   ///
    public:

        const TypeInfo* component_type() const;

        /**
         * \brief Returns the ordered array of entities this tag is applied to.
         */
        const EntityId* ent_range() const;

        std::size_t ent_range_len() const;

        const TagIndex_t* tag_indices() const;

        const TagCount_t* tag_counts() const;

        /**
         * \brief Returns a pointer to the tag buffer.
         */
        const void* tag_buffer() const;

        /**
         * \brief Returns the size of the tag buffer.
         */
        std::size_t tag_buffer_size() const;

        bool tags_single() const;

        bool tags_empty() const;

        //////////////////
        ///   Fields   ///
    private:

        const TypeInfo* _component_type;
        std::vector<EntityId> _ent_range;
        std::vector<TagIndex_t> _tag_indices;
        std::vector<TagCount_t> _tag_counts;
        void* _tag_buffer;
        std::size_t _tag_buffer_size;
    };
}
