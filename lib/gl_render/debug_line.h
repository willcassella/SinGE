#pragma once

#include "lib/base/math/vec3.h"
#include "lib/gl_render/glew.h"
#include "lib/resource/misc/color.h"

namespace sge
{
    namespace gl_render
    {
        /* These constants specify vertex attrib layout for debug lines. */
        constexpr GLuint DEBUG_LINE_POSITION_ATTRIB_LOCATION = 0;
        constexpr GLuint DEBUG_LINE_COLOR_ATTRIB_LOCATION = 1;
        constexpr const char* DEBUG_LINE_POSITION_ATTRIB_NAME = "v_position";
        constexpr const char* DEBUG_LINE_COLOR_ATTRIB_NAME = "v_color";

        struct DebugLineVert
        {
            Vec3 world_position;
            color::RGBF32 color_rgb;
        };
    }
}
