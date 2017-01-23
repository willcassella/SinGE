// PhysicsEntity.h
#pragma once

#include <memory>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <Engine/Component.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"

namespace sge
{
    namespace bullet_physics
    {
        struct PhysicsEntity : btMotionState
        {
            ////////////////////////
            ///   Constructors   ///
        public:

            PhysicsEntity(EntityId entity, BulletPhysicsSystem::Data& data);

            ///////////////////
            ///   Methods   ///
        public:

            void getWorldTransform(btTransform& worldTrans) const override;

            void setWorldTransform(const btTransform& worldTrans) override;

            //////////////////
            ///   Fields   ///
        public:

            EntityId entity;

            btVector3 position;

            btQuaternion rotation;

            btCompoundShape collider;

            std::unique_ptr<btBoxShape> box_collider;

            std::unique_ptr<btRigidBody> rigid_body;

        private:

            BulletPhysicsSystem::Data* _data;
        };
    }
}
