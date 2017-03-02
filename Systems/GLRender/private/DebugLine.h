// DebugLine.h
#pragma once

#include <Core/Math/Vec3.h>
#include <Resource/Misc/Color.h>
#include "glew.h"

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
