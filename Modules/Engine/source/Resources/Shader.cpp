// Shader.cpp

#include <iostream>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../../include/Engine/Resources/Shader.h"

SGE_REFLECT_TYPE(sge::VertexShader)
.flags(TF_SCRIPT_NOCONSTRUCT);

SGE_REFLECT_TYPE(sge::PixelShader)
.flags(TF_SCRIPT_NOCONSTRUCT);

namespace sge
{
	Shader::Shader()
	{
	}

	Shader::Shader(std::string source)
		: _source(std::move(source))
	{
	}

	void Shader::load(std::istream& file, std::size_t end)
	{
		// Read the file
		_source = std::string(end, ' ');
		file.read(&_source[0], end);
	}
}
