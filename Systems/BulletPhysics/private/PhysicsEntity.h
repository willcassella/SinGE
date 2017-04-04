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
            ////////////////////////
            ///   Constructors   ///
        public:

            PhysicsEntity(NodeId node, BulletPhysicsSystem::Data& data);
            ~PhysicsEntity();

            ///////////////////
            ///   Methods   ///
        private:

            void getWorldTransform(btTransform& world_trans) const override;

            void setWorldTransform(const btTransform& world_trans) override;

        public:

            void add_to_modified();

            void extern_set_transform(const btTransform& transform);

            //////////////////
            ///   Fields   ///
        public:

			NodeId node;

            btTransform transform;

            btCompoundShape collider;

			std::unique_ptr<btSphereShape> sphere_collider;

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
