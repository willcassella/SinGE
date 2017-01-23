// SystemData.h
#pragma once

#include <unordered_map>
#include <Engine/Component.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"
#include "PhysicsWorld.h"

namespace sge
{
    class CBoxCollider;
    class CRigidBody;
    class CTransform3D;

    namespace bullet_physics
    {
        struct PhysicsEntity;

        struct BulletPhysicsSystem::Data
        {
            ///////////////////
            ///   Methods   ///
        public:

            void add_box_collider(EntityId entity, const CBoxCollider& component);

            void remove_box_collider(EntityId entity);

            void add_rigid_body(EntityId entity, const CTransform3D& transform, const CRigidBody& component);

            void remove_rigid_body(EntityId entity);

        private:

            PhysicsEntity& get_or_create_physics_entity(EntityId entity);

            PhysicsEntity& get_physics_entity(EntityId entity);

            void post_modify_physics_entity(PhysicsEntity& phys_entity);

            //////////////////
            ///   Fields   ///
        public:

            std::unordered_map<EntityId, std::unique_ptr<PhysicsEntity>> physics_entities;

            std::vector<PhysicsEntity*> frame_transformed_entities;

            std::vector<PhysicsEntity*> new_rigid_bodies;

            /* NOTE: This must appear last, so that it is destroyed first. */
            PhysicsWorld phys_world;
        };
    }
}
