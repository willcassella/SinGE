// DebugLine.h
#pragma once

#include <Core/Math/Vec3.h>
#include "glew.h"

namespace sge
{
    namespace gl_render
    {
        /* These constants specify vertex attrib layout for debug lines. */
        static constexpr GLuint DEBUG_LINE_POSITION_ATTRIB_LOCATION = 0;
        static constexpr GLuint DEBUG_LINE_COLOR_ATTRIB_LOCATION = 1;
        static constexpr const char* DEBUG_LINE_POSITION_ATTRIB_NAME = "v_position";
        static constexpr const char* DEBUG_LINE_COLOR_ATTRIB_NAME = "v_color";

        struct DebugLineVert
        {
            //////////////////
            ///   Fields   ///
        public:

            Vec3 world_position;
            Vec3 color_rgb;
        };
    }
}
