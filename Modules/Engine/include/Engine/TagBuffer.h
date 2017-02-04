// TagBuffer.h
#pragma once

#include "Component.h"

namespace sge
{
    struct TypeInfo;
    using TagCount_t = uint8;

    struct SGE_ENGINE_API TagBuffer
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        /**
        * \brief Creates a tag buffer where each component may have one or more instances of non-empty tags.
        * \param component_type The component type these tags are being created for.
        * \param ord_entities An ordered array of the entities the tags are being created for.
        * \param tag_counts A count of the number of tags given for each entity.
        * \param num_ents The number of entities in the array, and the number of tag counts.
        * \param tag_buffer A buffer containing the tag values. Tags may be POD types only.
        * \param tag_buffer_size The size of the tag buffer.
        * \return An immutable tag buffer for the specified component type.
        */
        static TagBuffer create(
            const TypeInfo& component_type,
            const EntityId* ord_entities,
            const TagCount_t* tag_counts,
            std::size_t num_ents,
            const void* tag_buffer,
            std::size_t tag_buffer_size);

        /**
         * \brief Creates a tag buffer where each component has exactly one instance of a non-empty tag.
         * \param component_type The type of component this tag buffer is associated with.
         * \param ord_entities The entities for the components the tags are being applied to. This must be ordered and contain no duplicates.
         * \param num_ents The number of entities in the array (this is also the number of tags being added).
         * \param tag_buffer A pointer to the buffer containing the tags.
         * \param tag_buffer_size The size of the buffer containing the tags.
         * \return An immutble tag buffer for the specified component type.
         */
        static TagBuffer create_single(
            const TypeInfo& component_type,
            const EntityId* ord_entities,
            std::size_t num_ents,
            const void* tag_buffer,
            std::size_t tag_buffer_size);

        /**
         * \brief Creates a tag buffer where each component may have one or more empty tags associated with it.
         * \param component_type The component type these tags are being created for.
         * \param ord_entities The entities for the components the tags are being applied to. This must be ordered and contain no duplicates.
         * \param tag_counts A count of the number of tags given for each entity.
         * \param num_ents The number of entities, and the number of tag counts.
         * \return An immutable tag buffer for the specified component type.
         */
        static TagBuffer create_empty(
            const TypeInfo& component_type,
            const EntityId* ord_entities,
            const TagCount_t* tag_counts,
            std::size_t num_ents);

        /**
         * \brief Creates a tag buffer where each component has exactly one empty tag associated with it.
         * \param component_type The component type these tags are being created for.
         * \param ord_entities The entities for the components the tags are being applied to. This must be ordered and contain no duplicates.
         * \param num_ents A count of the number of tags given for each entity.
         * \return An immutable tag buffer for the specified componentt type.
         */
        static TagBuffer create_single_empty(
            const TypeInfo& component_type,
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

        const TypeInfo& component_type() const;

        /**
         * \brief Returns the ordered array of entities this tag is applied to.
         */
        const EntityId* get_ord_entities() const;

        std::size_t get_num_entities() const;

        const TagCount_t* get_tag_counts() const;

        /**
         * \brief Returns a pointer to the tag buffer.
         */
        const void* get_buffer() const;

        /**
         * \brief Returns the size of the tag buffer.
         */
        std::size_t buffer_size() const;

        bool tags_single() const;

        bool tags_empty() const;

        //////////////////
        ///   Fields   ///
    private:

        const TypeInfo* _component_type;
        std::vector<EntityId> _ord_entities;
        std::vector<TagCount_t> _tag_counts;
        void* _tag_buffer;
        std::size_t _tag_buffer_size;
    };
}
