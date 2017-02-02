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

            void turn(Angle amount);

            //////////////////
            ///   Fields   ///
        public:

            btPairCachingGhostObject ghost_object;

        private:

            PhysicsEntity* _phys_entity;
            Angle _turn_amount;
            Vec3 _walk_dir;
        };
    }
}
