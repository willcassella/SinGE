// Archive.h
#pragma once

#include "ArchiveReader.h"
#include "ArchiveWriter.h"

namespace sge
{
    class SGE_CORE_API Archive
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        virtual ~Archive() = default;

        ///////////////////
        ///   Methods   ///
    public:

        /**
         * \brief Writes to the root node of this archive.
         */
        virtual ArchiveWriter* write_root() = 0;

        /**
         * \brief Reads from the root node of this archive.
         */
        virtual ArchiveReader* read_root() const = 0;

        /**
         * \brief Trys to load this archive from a file.
         * \param path The path to the file to load this archive from.
         * \return Whether the operation succeeded.
         */
        virtual bool from_file(const char* path) = 0;

        /**
         * \brief Trys to save this archive to a file.
         * \param path The path to the file to save this archive to.
         * \return Whether the operation succeeded.
         */
        virtual bool to_file(const char* path) const = 0;

        /**
         * \brief Serializes the given value to the root node of this archive.
         * \param rootValue The value to serialize to the root node.
         */
        template <typename T>
        void serialize_root(const T& rootValue)
        {
            auto* writer = this->write_root();
            sge::to_archive(rootValue, *writer);
            writer->pop();
        };

        /**
         * \brief Deserializes the given value from the root node of this archive.
         * \param rootValue The value to deserialize from the root node.
         */
        template <typename T>
        void deserialize_root(T& rootValue) const
        {
            auto* reader = this->read_root();
            sge::from_archive(rootValue, *reader);
            reader->pop();
        }
    };
}
