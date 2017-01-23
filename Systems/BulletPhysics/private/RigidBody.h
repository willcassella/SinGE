// RigidBody.h
#pragma once

#include <memory>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "PhysTransform.h"
#include "Collider.h"

namespace sge
{
    namespace bullet_physics
    {
        struct RigidBody
        {
            ////////////////////////
            ///   Constructors   ///
        public:

            RigidBody()
            {
            }

            //////////////////
            ///   Fields   ///
        private:

            PhsTransform _transform;
            std::unique_ptr<btRigidBody> _rigid_body;
        };
    }
}
