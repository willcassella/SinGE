// SystemData.h
#pragma once

#include <set>
#include <unordered_map>
#include <Engine/Component.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"
#include "PhysicsWorld.h"

namespace sge
{
    class CTransform3D;
    class CBoxCollider;
    class CCapsuleCollider;
    class CRigidBody;
    class CCharacterController;

    namespace bullet_physics
    {
        class PhysicsEntity;
        class CharacterController;

        /**
         * \brief Comparison function object for PhysicsEntity pointers (maintains EntityId order).
         */
        struct PhysicsEntityLess
        {
            bool operator()(const PhysicsEntity* lhs, const PhysicsEntity* rhs);
        };

        struct BulletPhysicsSystem::Data
        {
            ///////////////////
            ///   Methods   ///
        public:

            void add_box_collider(EntityId entity, const CBoxCollider& component);

            void remove_box_collider(EntityId entity);

            void add_capsule_collider(EntityId entity, const CCapsuleCollider& component);

            void remove_capsule_collider(EntityId entity);

            void add_rigid_body(EntityId entity, const CTransform3D& transform, const CRigidBody& component);

            void remove_rigid_body(EntityId entity);

            void add_character_controller(
                EntityId entity,
                const CTransform3D& transform,
                const CCharacterController& character_controller);

            void remove_character_contoller(EntityId entity);

            PhysicsEntity& get_or_create_physics_entity(EntityId entity);

            PhysicsEntity& get_physics_entity(EntityId entity);

            void post_modify_physics_entity(PhysicsEntity& phys_entity);

            //////////////////
            ///   Fields   ///
        public:

            std::unordered_map<EntityId, std::unique_ptr<PhysicsEntity>> physics_entities;

            std::set<PhysicsEntity*, PhysicsEntityLess> frame_transformed_entities;

            /* NOTE: This must appear last, so that it is destroyed first. */
            PhysicsWorld phys_world;
        };
    }
}
