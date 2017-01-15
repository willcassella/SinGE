// Material.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Resource/Archives/JsonArchive.h>
#include <Resource/Interfaces/IFromFile.h>
#include "../../include/Engine/Resources/Material.h"

SGE_REFLECT_TYPE(sge::Material)
.flags(TF_SCRIPT_NOCONSTRUCT)
.implements<IToArchive>()
.implements<IFromArchive>()
.implements<IFromFile>();

namespace sge
{
	void Material::ParamTable::to_archive(ArchiveWriter& writer) const
	{
		for (const auto& param : float_params)
		{
			writer.object_member(param.first.c_str(), param.second);
		}

		for (const auto& param : vec2_params)
		{
			writer.object_member(param.first.c_str(), param.second);
		}

		for (const auto& param : vec3_params)
		{
			writer.object_member(param.first.c_str(), param.second);
		}

		for (const auto& param : vec4_params)
		{
			writer.object_member(param.first.c_str(), param.second);
		}

		for (const auto& param : texture_params)
		{
			writer.object_member(param.first.c_str(), param.second);
		}
	}

	void Material::ParamTable::from_archive(ArchiveReader& reader)
	{
		float_params.clear();
		vec2_params.clear();
		vec3_params.clear();
		vec4_params.clear();
		texture_params.clear();

		reader.enumerate_object_members([this, &reader](const char* name)
		{
			// If this reader holds a numeric value
			if (reader.is_number())
			{
				float value;
				reader.number(value);
				this->float_params.insert(std::make_pair(name, value));
			}
			// If this reader holds a string
			else if (reader.is_string())
			{
				std::size_t len;
				reader.string_size(len);
				std::string tex;
				tex.assign(len, 0);
				reader.string(&tex[0], len);
				this->texture_params.insert(std::make_pair(name, std::move(tex)));
			}
			else
			{
				std::size_t size;
				assert(reader.array_size(size));

				if (size == 2)
				{
					Vec2 vec;
					sge::from_archive(vec, reader);
					this->vec2_params.insert(std::make_pair(name, vec));
				}
				else if (size == 3)
				{
					Vec3 vec;
					sge::from_archive(vec, reader);
					this->vec3_params.insert(std::make_pair(name, vec));
				}
				else if (size == 4)
				{
					Vec4 vec;
					sge::from_archive(vec, reader);
					this->vec4_params.insert(std::make_pair(name, vec));
				}
				else
				{
					// Reader holds vec of unsupported size
					assert(false);
				}
			}
		});
	}

	bool Material::from_file(const char* path)
	{
		JsonArchive archive;
		if (!archive.from_file(path))
		{
			return false;
		}

		archive.deserialize_root(*this);
		return true;
	}

	void Material::to_archive(ArchiveWriter& writer) const
	{
		writer.object_member("vertex_shader", _vertex_shader);
		writer.object_member("pixel_shader", _pixel_shader);
		writer.object_member("params", _param_table);
	}

	void Material::from_archive(ArchiveReader& reader)
	{
		reader.object_member("vertex_shader", _vertex_shader);
		reader.object_member("pixel_shader", _pixel_shader);
		reader.object_member("params", _param_table);
	}
}
