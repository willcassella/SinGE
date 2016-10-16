// Texture.h
#pragma once

#include <Resource/Image.h>
#include "../config.h"

namespace sge
{
	struct SGE_ENGINE_API Texture
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		Texture(const std::string& path);

		//////////////////
		///   Fields   ///
	public:

		Image image;
	};
}
