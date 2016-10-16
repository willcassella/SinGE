// Material.cpp

#include "../../include/Engine/Resources/Material.h"
#include <Core/Reflection/ReflectionBuilder.h>

SGE_REFLECT_TYPE(sge::Material)
.flags(TF_SCRIPT_NOCONSTRUCT);

namespace sge
{
	Material::Material(const std::string& /*path*/)
	{
		// TODO: Actualy load a file
		_vertex_shader = "engine_content/shaders/basic.vert";
		_pixel_shader = "engine_content/shaders/basic.frag";
		_param_table.texture_params["diffuse"] = "game_content/tex.jpg";
	}
}
