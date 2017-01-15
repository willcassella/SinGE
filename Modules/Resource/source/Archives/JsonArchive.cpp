// JsonArchive.cpp

#include <cstdio>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/writer.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../../include/Resource/Interfaces/IFromFile.h"
#include "../../include/Resource/Archives/JsonArchive.h"
#include "../../private/JsonArchiveWriter.h"
#include "../../private/JsonArchiveReader.h"

SGE_REFLECT_TYPE(sge::JsonArchive)
.implements<IFromFile>();

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

	ArchiveReader* JsonArchive::read_root() const
	{
		return new JsonArchiveReader{ _data->doc };
	}

	ArchiveWriter* JsonArchive::write_root()
	{
		return new JsonArchiveWriter{ _data->doc, _data->doc.GetAllocator() };
	}

	bool JsonArchive::to_file(const char* path) const
	{
		std::FILE* file = std::fopen(path, "wb");
		char writeBuffer[65536];

		rapidjson::FileWriteStream out{ file, writeBuffer, sizeof(writeBuffer) };
		rapidjson::Writer<rapidjson::FileWriteStream> writer{ out };

		_data->doc.Accept(writer);

		std::fclose(file);
		return true;
	}

	bool JsonArchive::from_file(const char* path)
	{
		std::FILE* file = std::fopen(path, "rb");
		if (!file)
		{
			return false;
		}

		char readBuffer[65536];
		rapidjson::FileReadStream in(file, readBuffer, sizeof(readBuffer));

		_data->doc.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(in);

		std::fclose(file);
		return true;
	}

	std::string JsonArchive::to_string() const
	{
		rapidjson::StringBuffer buffer;
		buffer.Clear();

		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		_data->doc.Accept(writer);

		return buffer.GetString();
	}

	void JsonArchive::from_string(const char* str)
	{
		_data->doc.Parse(str);
	}
}
