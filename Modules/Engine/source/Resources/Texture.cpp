// Texture.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../include/Engine/Resources/Texture.h"

SGE_REFLECT_TYPE(sge::Texture)
.flags(TF_SCRIPT_NOCONSTRUCT);

namespace sge
{
	Texture::Texture(const std::string& path)
		: image(path)
	{
	}
}
