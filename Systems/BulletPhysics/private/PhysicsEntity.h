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

            void extern_set_transform(const btTransform& transform, const btVector3& scale);

            //////////////////
            ///   Fields   ///
        public:

			NodeId node;
            btTransform transform;
            btCompoundShape collider;

			// Not the best way to do this, but it works
			// I think what I'd rather have is seperate sets of things that correspond to the same object
        	std::unique_ptr<btSphereShape> sphere_collider;
            std::unique_ptr<btBoxShape> box_collider;
            std::unique_ptr<btCapsuleShape> capsule_collider;
			std::unique_ptr<btScaledBvhTriangleMeshShape> static_mesh_collider;

			std::unique_ptr<btCollisionObject> collision_object;
        	std::unique_ptr<btPairCachingGhostObject> ghost_object;
            std::unique_ptr<btRigidBody> rigid_body;
            std::unique_ptr<CharacterController> character_controller;

        private:

            BulletPhysicsSystem::Data* _data;
        };
    }
}
