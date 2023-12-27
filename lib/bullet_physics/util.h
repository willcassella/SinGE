#pragma once

#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>

#include "lib/base/math/vec3.h"
#include "lib/base/math/quat.h"

namespace sge
{
    namespace bullet_physics
    {
        inline btVector3 to_bullet(const Vec3& vec)
        {
            return btVector3{ vec.x(), vec.y(), vec.z() };
        }

        inline btQuaternion to_bullet(const Quat& quat)
        {
            return btQuaternion{ quat.x(), quat.y(), quat.z(), quat.w() };
        }

        inline Vec3 from_bullet(const btVector3& vec)
        {
            return Vec3{ vec.x(), vec.y(), vec.z() };
        }

        inline Quat from_bullet(const btQuaternion& quat)
        {
            return Quat{ quat.x(), quat.y(), quat.z(), quat.w() };
        }
    }
}
