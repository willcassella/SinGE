// Material.h
#pragma once

#include <unordered_map>
#include <Core/Math/Vec4.h>
#include "../config.h"

namespace sge
{
	struct SGE_ENGINE_API Material
	{
		SGE_REFLECTED_TYPE;

		struct ParamTable
		{
			///////////////////
			///   Methods   ///
		public:

			void to_archive(ArchiveWriter& writer) const;

			void from_archive(const ArchiveReader& reader);

			//////////////////
			///   Fields   ///
		public:

			std::unordered_map<std::string, float> float_params;
			std::unordered_map<std::string, Vec2> vec2_params;
			std::unordered_map<std::string, Vec3> vec3_params;
			std::unordered_map<std::string, Vec4> vec4_params;
			std::unordered_map<std::string, std::string> texture_params;
		};

		///////////////////
		///   Methods   ///
	public:

		void to_archive(ArchiveWriter& writer) const;

		void from_archive(const ArchiveReader& reader);

		const std::string& vertex_shader() const
		{
			return _vertex_shader;
		}

		const std::string& pixel_shader() const
		{
			return _pixel_shader;
		}

		const ParamTable& param_table() const
		{
			return _param_table;
		}

		void set_param(std::string name, float value)
		{
			_param_table.float_params[std::move(name)] = value;
		}

		void set_param(std::string name, Vec2 value)
		{
			_param_table.vec2_params[std::move(name)] = value;
		}

		void set_param(std::string name, Vec3 value)
		{
			_param_table.vec3_params[std::move(name)] = value;
		}

		void set_param(std::string name, Vec4 value)
		{
			_param_table.vec4_params[std::move(name)] = value;
		}

		void set_param(std::string name, std::string value)
		{
			_param_table.texture_params[std::move(name)] = std::move(value);
		}

		//////////////////
		///   Fields   ///
	private:

		std::string _vertex_shader;
		std::string _pixel_shader;
		ParamTable _param_table;
	};
}
