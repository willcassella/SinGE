// PhysicsEntity.h
#pragma once

#include <memory>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <Engine/Component.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"

namespace sge
{
    namespace bullet_physics
    {
        class CharacterController;
        struct LightmaskVolumeCollider;

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

            int get_user_index_1() const;

            void set_user_index_1(int value);

            int get_user_index_2() const;

            void set_user_index_2(int value);

            //////////////////
            ///   Fields   ///
        public:

            NodeId node;
            btTransform transform;
            btCompoundShape collider;
            BulletPhysicsSystem::Data* phys_data;

            // Not the best way to do this, but it works
            // I think what I'd rather have is seperate sets of things that correspond to the same object
            std::unique_ptr<btSphereShape> sphere_collider;
            std::unique_ptr<btBoxShape> box_collider;
            std::unique_ptr<btCapsuleShape> capsule_collider;
            std::unique_ptr<btScaledBvhTriangleMeshShape> static_mesh_collider;
            std::unique_ptr<LightmaskVolumeCollider> lightmask_volume_collider;

            std::unique_ptr<btPairCachingGhostObject> lightmask_volume_ghost;
            std::unique_ptr<btPairCachingGhostObject> level_portal_ghost;
            std::unique_ptr<btCollisionObject> collision_object;
            std::unique_ptr<btRigidBody> rigid_body;
            std::unique_ptr<CharacterController> character_controller;

        private:

            int _user_index_1_value;
            int _user_index_2_value;
        };
    }
}
