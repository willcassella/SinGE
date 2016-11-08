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
		 * \param func The function to call with a writer for the root node of this archive.
		 */
		virtual void set_root(FunctionView<void(ArchiveWriter& rootWriter)> func) = 0;

		/**
		 * \brief Reads from the root node of this archive.
		 * \param func The function to call with a reader for the root node of this archive.
		 */
		virtual void get_root(FunctionView<void(const ArchiveReader& rootReader)> func) const = 0;

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
			set_root([&](ArchiveWriter& rootWriter) {
				to_archive(rootValue, rootWriter);
			});
		};

		/**
		 * \brief Deserializes the given value from the root node of this archive.
		 * \param rootValue The value to deserialize from the root node.
		 */
		template <typename T>
		void deserialize_root(T& rootValue) const
		{
			get_root([&](const ArchiveReader& rootReader) {
				from_archive(rootValue, rootReader);
			});
		}
	};
}
