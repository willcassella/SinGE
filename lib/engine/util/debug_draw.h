#pragma once

#include "lib/base/math/vec3.h"
#include "lib/engine/build.h"
#include "lib/resource/misc/color.h"

namespace sge
{
    struct DebugLine
    {
        /* Starting point for the line in world coordintes. */
        Vec3 world_start;

        /* Ending point for the line in world coordinates. */
        Vec3 world_end;

        /* Color of the line. */
        color::RGBF32 color;
    };
}
