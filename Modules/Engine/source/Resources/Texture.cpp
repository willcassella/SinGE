// Texture.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Interfaces/IToArchive.h>
#include <Resource/Interfaces/IFromFile.h>
#include "../../include/Engine/Resources/Texture.h"

SGE_REFLECT_TYPE(sge::Texture)
.flags(TF_SCRIPT_NOCONSTRUCT)
.implements<IToArchive>()
.implements<IFromFile>();

namespace sge
{
	Texture::Texture()
        : _color_space(ColorSpace::S_RGB)
	{
	}

    Texture::Texture(const std::string& path)
        : _color_space(ColorSpace::S_RGB),
        image(path)
	{
	}

	bool Texture::from_file(const char* path)
	{
		return image.from_file(path);
	}

	void Texture::to_archive(ArchiveWriter& writer) const
	{
		writer.object_member("image", image);
	}
}
