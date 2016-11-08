// Material.cpp

#include "../../include/Engine/Resources/Material.h"
#include <Core/Reflection/ReflectionBuilder.h>

SGE_REFLECT_TYPE(sge::Material)
.implements<IToArchive>()
.implements<IFromArchive>()
.flags(TF_SCRIPT_NOCONSTRUCT);

namespace sge
{
	void Material::ParamTable::to_archive(ArchiveWriter& writer) const
	{
		for (const auto& param : float_params)
		{
			writer.push_object_member(param.first.c_str(), param.second);
		}

		for (const auto& param : vec2_params)
		{
			writer.push_object_member(param.first.c_str(), param.second);
		}

		for (const auto& param : vec3_params)
		{
			writer.push_object_member(param.first.c_str(), param.second);
		}

		for (const auto& param : vec4_params)
		{
			writer.push_object_member(param.first.c_str(), param.second);
		}

		for (const auto& param : texture_params)
		{
			writer.push_object_member(param.first.c_str(), param.second);
		}
	}

	void Material::ParamTable::from_archive(const ArchiveReader& reader)
	{
		reader.enumerate_object_members([this](const char* name, const ArchiveReader& paramReader)
		{
			// If this reader holds a numeric value
			if (paramReader.is_value())
			{
				float value;
				assert(paramReader.value(value));
				this->float_params.insert(std::make_pair(name, value));
			}
			// If this reader holds a string
			else if (paramReader.is_string())
			{
				const char* str;
				std::size_t len;
				assert(paramReader.string(str, len));
				this->texture_params.insert(std::make_pair(name, std::string{ str, len }));
			}
			else
			{
				std::size_t size;
				assert(paramReader.array_size(size));

				if (size == 2)
				{
					Vec2 vec;
					sge::from_archive(vec, paramReader);
					this->vec2_params.insert(std::make_pair(name, vec));
				}
				else if (size == 3)
				{
					Vec3 vec;
					sge::from_archive(vec, paramReader);
					this->vec3_params.insert(std::make_pair(name, vec));
				}
				else if (size == 4)
				{
					Vec4 vec;
					sge::from_archive(vec, paramReader);
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

	void Material::to_archive(ArchiveWriter& writer) const
	{
		writer.push_object_member("vertex_shader", _vertex_shader);
		writer.push_object_member("pixel_shader", _pixel_shader);
		writer.push_object_member("params", _param_table);
	}

	void Material::from_archive(const ArchiveReader& reader)
	{
		reader.pull_object_member("vertex_shader", _vertex_shader);
		reader.pull_object_member("pixel_shader", _pixel_shader);
		reader.pull_object_member("params", _param_table);
	}
}
