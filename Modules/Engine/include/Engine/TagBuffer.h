// TagBuffer.h
#pragma once

#include "Component.h"

namespace sge
{
    struct TypeInfo;

    struct SGE_ENGINE_API TagBuffer
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        /**
        * \brief Creates a component-type specific tag buffer.
        * \param component_type The component type these tags are being created for.
        * \param ord_entities An ordered array of the entities the tags are being created for.
        * \param tags The tags values to copy.
        * \param tag_size The size of each tag.
        * \param num_tags The number of tags to create.
        * \return A tag buffer for the specified component type.
        */
        static TagBuffer create(
            const TypeInfo& component_type,
            const EntityId* ord_entities,
            const void* tags,
            std::size_t tag_size,
            std::size_t num_tags);

        /**
         * \brief Creates a component-type specific tag buffer, by copying a single tag.
         * \param component_type The component type these tags are being created for.
         * \param ord_entities An ordered arrays of the entities the tags are being created for.
         * \param single_tag The single tag instance to be copied for each component instance.
         * \param tag_size The size of the tag object.
         * \param num_tags The number of tags being created.
         * \return A tag buffer for the specified component type.
         */
        static TagBuffer create_from_single(
            const TypeInfo& component_type,
            const EntityId* ord_entities,
            const void* single_tag,
            std::size_t tag_size,
            std::size_t num_tags);

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

        /**
         * \brief Returns the number of tags.
         */
        std::size_t num_tags() const;

        /**
         * \brief Returns a pointer to the tag buffer.
         */
        const void* get_buffer() const;

        /**
         * \brief Returns the size of the tag buffer.
         */
        std::size_t buffer_size() const;

        //////////////////
        ///   Fields   ///
    private:

        const TypeInfo* _component_type;
        std::vector<EntityId> _ord_entities;
        void* _buffer;
        std::size_t _size;
    };
}
