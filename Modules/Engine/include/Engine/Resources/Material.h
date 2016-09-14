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

		///////////////////
		///   Methods   ///
	public:



		////////////////
		///   Data   ///
	private:

		std::string _vertex_shader;
		std::string _pixel_shader;
		std::unordered_map<std::string, float> _float_params;
		std::unordered_map<std::string, Vec2> _vec2_params;
		std::unordered_map<std::string, Vec3> _vec3_params;
		std::unordered_map<std::string, Vec4> _vec4_params;
		std::unordered_map<std::string, std::string> _texture_params;
	};
}
