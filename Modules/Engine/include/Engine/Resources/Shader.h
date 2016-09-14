// Shader.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "../config.h"

namespace sge
{
	struct SGE_ENGINE_API Shader
	{
		SGE_REFLECTED_TYPE;

		enum class Type
		{
			VERTEX_SHADER,
			PIXEL_SHADER
		};
		
		////////////////////////
		///   Constructors   ///
	public:

		Shader(Type type, std::string source);

		///////////////////
		///   Methods   ///
	public:

		Type get_shader_type() const
		{
			return _type;
		}

		const std::string& get_source() const
		{
			return _source;
		}

		////////////////
		///   Data   ///
	private:

		Type _type;
		std::string _source;
	};
}
