// Shader.cpp

#include "../../include/Engine/Resources/Shader.h"

SGE_REFLECT_TYPE(sge::Shader);

namespace sge
{
	Shader::Shader(Type type, std::string source)
		: _type{ type }, _source{ std::move(source) }
	{
	}
}
