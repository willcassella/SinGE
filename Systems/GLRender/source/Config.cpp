// Config.cpp

#include <Core/IO/ArchiveReader.h>
#include <Core/Interfaces/IFromArchive.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/GLRender/Config.h"

SGE_REFLECT_TYPE(sge::gl_render::Config)
.implements<IFromArchive>();

namespace sge
{
    namespace gl_render
    {
        Config::Config()
            : viewport_width(0),
            viewport_height(0)
        {
        }

        void Config::from_archive(ArchiveReader& reader)
        {
            reader.object_member("viewport_width", viewport_width);
            reader.object_member("viewport_height", viewport_height);
            reader.object_member("viewport_vert_shader", viewport_vert_shader);
            reader.object_member("scene_shader", scene_shader);
            reader.object_member("post_shader", post_shader);
            reader.object_member("debug_line_vert_shader", debug_line_vert_shader);
            reader.object_member("debug_line_frag_shader", debug_line_frag_shader);
            reader.object_member("missing_material", missing_material);
            reader.object_member("missing_mesh", missing_mesh);
        }

        bool Config::validate() const
        {
            if (viewport_width <= 0)
            {
                return false;
            }
            if (viewport_height <= 0)
            {
                return false;
            }
            if (viewport_vert_shader.empty())
            {
                return false;
            }
            if (scene_shader.empty())
            {
                return false;
            }
            if (post_shader.empty())
            {
                return false;
            }
            if (debug_line_vert_shader.empty())
            {
                return false;
            }
            if (debug_line_frag_shader.empty())
            {
                return false;
            }
            if (missing_material.empty())
            {
                return false;
            }
            if (missing_mesh.empty())
            {
                return false;
            }

            return true;
        }
    }
}
