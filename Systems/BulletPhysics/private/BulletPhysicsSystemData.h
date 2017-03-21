// SystemData.h
#pragma once

#include <map>
#include <Engine/Component.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"
#include "PhysicsWorld.h"

namespace sge
{
    struct CBoxCollider;
    struct CCapsuleCollider;
    struct CRigidBody;
    struct CCharacterController;

    namespace bullet_physics
    {
        class PhysicsEntity;
        class CharacterController;

		struct PhysTransformedNode
		{
			Vec3 world_transform;
			Quat world_rotation;
		};

        struct BulletPhysicsSystem::Data
        {
            ///////////////////
            ///   Methods   ///
        public:

            void add_box_collider(NodeId node, const CBoxCollider& component);

            void remove_box_collider(NodeId node);

            void add_capsule_collider(NodeId node, const CCapsuleCollider& component);

            void remove_capsule_collider(NodeId node);

            void add_rigid_body(const Node& node, const CRigidBody& component);

            void remove_rigid_body(NodeId node);

            void add_character_controller(
                const Node& node,
                const CCharacterController& character_controller);

            void remove_character_contoller(NodeId node);

            PhysicsEntity& get_or_create_physics_entity(NodeId node);

            PhysicsEntity* get_physics_entity(NodeId node);

            void post_modify_physics_entity(PhysicsEntity& phys_entity);

            //////////////////
            ///   Fields   ///
        public:

            std::map<NodeId, std::unique_ptr<PhysicsEntity>> physics_entities;

			std::vector<NodeId> frame_transformed_nodes;
			std::vector<PhysTransformedNode> frame_transformed_node_transforms;

            /* NOTE: This must appear last, so that it is destroyed first. */
            PhysicsWorld phys_world;
        };
    }
}
