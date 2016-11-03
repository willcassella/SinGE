// JsonArchive.h
#pragma once

#include <memory>
#include <Core/IO/ArchiveWriter.h>
#include <Core/IO/ArchiveReader.h>
#include "../config.h"

namespace sge
{
	struct SGE_RESOURCE_API JsonArchive
	{
		struct Data;

		////////////////////////
		///   Constructors   ///
	public:

		JsonArchive();
		~JsonArchive();

		///////////////////
		///   Methods   ///
	public:

		void set_root(FunctionView<void(ArchiveWriter& rootWriter)> func);

		void get_root(FunctionView<void(const ArchiveReader& rootReader)> func) const;

		void save(const char* path) const;

		void load(const char* path);

		std::string dump_string() const;

		void parse_string(const char* str);

		template <typename T>
		void serialize_root(const T& value)
		{
			this->set_root([&](ArchiveWriter& rootWriter) {
				to_archive(value, rootWriter);
			});
		}

		template <typename T>
		void deserialize_root(T& value) const
		{
			this->get_root([&](const ArchiveReader& rootReader) {
				from_archive(value, rootReader);
			});
		}

		//////////////////
		///   Fields   ///
	private:

		std::unique_ptr<Data> _data;
	};
}
