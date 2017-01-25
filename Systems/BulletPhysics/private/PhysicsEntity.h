// PhysicsEntity.h
#pragma once

#include <memory>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <Engine/Component.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"

namespace sge
{
    namespace bullet_physics
    {
        class CharacterController;

        class PhysicsEntity final : public btMotionState
        {
        public:

            using Features_t = uint8;
            enum : Features_t
            {
                FT_NONE = 0,
                FT_TRANSFORM_3D = (1 << 0),
                FT_SENSOR = (1 << 1),
            };

            ////////////////////////
            ///   Constructors   ///
        public:

            PhysicsEntity(EntityId entity, BulletPhysicsSystem::Data& data);
            ~PhysicsEntity();

            ///////////////////
            ///   Methods   ///
        private:

            void getWorldTransform(btTransform& world_trans) const override;

            void setWorldTransform(const btTransform& world_trans) override;

        public:

            void add_to_modified();

            //////////////////
            ///   Fields   ///
        public:

            Features_t features;

            EntityId entity;

            btTransform transform;

            btCompoundShape collider;

            std::unique_ptr<btBoxShape> box_collider;

            std::unique_ptr<btCapsuleShape> capsule_collider;

            std::unique_ptr<btRigidBody> rigid_body;

            std::unique_ptr<btPairCachingGhostObject> ghost_object;

            std::unique_ptr<CharacterController> character_controller;

        private:

            BulletPhysicsSystem::Data* _data;
        };
    }
}
