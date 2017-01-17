// Collider.h
#pragma once

#include <btBulletCollisionCommon.h>
#include <Engine/Component.h>

namespace sge
{
    namespace bullet_physics
    {
        struct Collider
        {
            //////////////////
            ///   Fields   ///
        public:

            btCompoundShape compound;
        };
    }
}
