// CharacterController.h
#pragma once

#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <Core/Math/Vec3.h>

namespace sge
{
    class CCharacterController;

    namespace bullet_physics
    {
        class PhysicsEntity;

        class CharacterController final : public btKinematicCharacterController
        {
            ////////////////////////
            ///   Constructors   ///
        public:

            CharacterController(
                PhysicsEntity& phys_enitty,
                const CCharacterController& component);

            ///////////////////
            ///   Methods   ///
        public:

            void updateAction(btCollisionWorld* world, btScalar deltaTimeStep) override;

            void walk(Vec2 dir);

            //////////////////
            ///   Fields   ///
        private:

            Vec3 walk_dir;
            PhysicsEntity* _phys_entity;
        };
    }
}
