// JsonArchive.cpp

#include <cstdio>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/writer.h>
#include "../../include/Resource/Archives/JsonArchive.h"
#include "../../private/JsonArchiveWriter.h"
#include "../../private/JsonArchiveReader.h"

namespace sge
{
	struct JsonArchive::Data
	{
		rapidjson::Document doc;
	};

	JsonArchive::JsonArchive()
	{
		_data = std::make_unique<JsonArchive::Data>();
	}

	JsonArchive::~JsonArchive()
	{
	}

	void JsonArchive::get_root(FunctionView<void(const ArchiveReader& rootReader)> func) const
	{
		JsonArchiveReader rootReader{ _data->doc };
		func(rootReader);
	}

	void JsonArchive::set_root(FunctionView<void(ArchiveWriter& rootWriter)> func)
	{
		JsonArchiveWriter rootWriter{ _data->doc, _data->doc.GetAllocator() };
		func(rootWriter);
	}

	void JsonArchive::save(const char* path) const
	{
		std::FILE* file = std::fopen(path, "wb");
		char writeBuffer[65536];

		rapidjson::FileWriteStream out{ file, writeBuffer, sizeof(writeBuffer) };
		rapidjson::Writer<rapidjson::FileWriteStream> writer{ out };

		_data->doc.Accept(writer);

		std::fclose(file);
	}

	void JsonArchive::load(const char* path)
	{
		std::FILE* file = std::fopen(path, "rb");
		char readBuffer[65536];

		rapidjson::FileReadStream in(file, readBuffer, sizeof(readBuffer));

		_data->doc.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(in);

		std::fclose(file);
	}
}
