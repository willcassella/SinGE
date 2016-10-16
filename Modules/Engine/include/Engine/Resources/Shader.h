// Shader.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "../config.h"

namespace sge
{
	struct SGE_ENGINE_API Shader
	{
		////////////////////////
		///   Constructors   ///
	public:

		Shader();
		Shader(std::string source);

		///////////////////
		///   Methods   ///
	public:

		void load(std::istream& file, std::size_t end);

		const std::string& source() const
		{
			return _source;
		}

		//////////////////
		///   Fields   ///
	private:

		std::string _source;
	};

	struct VertexShader : Shader
	{
		SGE_REFLECTED_TYPE;
		using Shader::Shader;
	};

	struct PixelShader : Shader
	{
		SGE_REFLECTED_TYPE;
		using Shader::Shader;
	};
}
