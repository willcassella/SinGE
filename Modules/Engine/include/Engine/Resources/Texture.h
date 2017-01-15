// Texture.h
#pragma once

#include <Resource/Resources/Image.h>
#include "../config.h"

namespace sge
{
	struct SGE_ENGINE_API Texture
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		Texture();
		Texture(const std::string& path);

		///////////////////
		///   Methods   ///
	public:

		bool from_file(const char* path);

		void to_archive(ArchiveWriter& writer) const;

		//////////////////
		///   Fields   ///
	public:

		Image image;
	};
}
