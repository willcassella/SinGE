#pragma once

#include "lib/base/reflection/reflection.h"
#include "lib/gl_render/build.h"

namespace sge
{
    class ArchiveReader;

    namespace gl_render
    {
        struct SGE_GL_RENDER_API Config
        {
            SGE_REFLECTED_TYPE;

            Config();

            void from_archive(ArchiveReader& reader);

            bool validate() const;

            int viewport_width;
            int viewport_height;
            std::string viewport_vert_shader;
            std::string scene_shader;
            std::string post_shader;
            std::string debug_line_vert_shader;
            std::string debug_line_frag_shader;
            std::string missing_material;
            std::string missing_mesh;
        };
    }
}
