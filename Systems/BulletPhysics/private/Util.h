// Util.h
#pragma once

#include <Engine/Components/CTransform3D.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>
#include <Core/Math/Vec3.h>
#include <Core/Math/Quat.h>

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

        inline void to_bullet(btTransform& bt_trans, const CTransform3D& sge_trans)
        {
            bt_trans.setOrigin(to_bullet(sge_trans.get_world_position()));
            bt_trans.setRotation(to_bullet(sge_trans.get_world_rotation()));
        }

        inline void from_bullet(CTransform3D& sge_trans, const btTransform& bt_trans)
        {
            // TODO: Set world transforms instead of local
            sge_trans.set_local_position(from_bullet(bt_trans.getOrigin()));
            sge_trans.set_local_rotation(from_bullet(bt_trans.getRotation()));
        }
    }
}
