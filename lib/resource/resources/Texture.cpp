#include "lib/base/interfaces/to_archive.h"
#include "lib/base/reflection/reflection_builder.h"
#include "lib/base/util/string_utils.h"
#include "lib/resource/interfaces/from_file.h"
#include "lib/resource/resources/texture.h"

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
        : Texture()
    {
        from_file(path.c_str());
    }

    bool Texture::from_file(const char* path)
    {
        // TODO: Should have better method than this
        if (string_ends_with(path, ".tif"))
        {
            _color_space = ColorSpace::RGB;
        }
        else
        {
            _color_space = ColorSpace::S_RGB;
        }

        return image.from_file(path);
    }

    void Texture::to_archive(ArchiveWriter& writer) const
    {
        writer.object_member("image", image);
    }
}
