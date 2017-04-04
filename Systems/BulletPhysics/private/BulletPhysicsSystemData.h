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

            PhysicsEntity& get_or_create_physics_entity(NodeId node);

            PhysicsEntity* get_physics_entity(NodeId node);

			void post_add_physics_entity_element(PhysicsEntity& phys_entity);

            void post_remove_physics_entity_element(PhysicsEntity& phys_entity);

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
