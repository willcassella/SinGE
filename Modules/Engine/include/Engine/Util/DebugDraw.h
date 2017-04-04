// DebugDraw.h
#pragma once

#include <Core/Math/Vec3.h>
#include <Resource/Misc/Color.h>
#include "../config.h"

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
