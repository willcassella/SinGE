// RigidBody.h
#pragma once

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

            ///////////////////
            ///   Methods   ///
        public:



            //////////////////
            ///   Fields   ///
        private:

            PhsTransform _transform;
            alignas(btRigidBody) byte _rigid_body_buffer[sizeof(btRigidBody)];
        };
    }
}
