// BulletPhysicsSystemData.cpp

#include <Engine/Components/Physics/CBoxCollider.h>
#include <Engine/Components/Physics/CCapsuleCollider.h>
#include <Engine/Components/Physics/CRigidBody.h>
#include "../private/BulletPhysicsSystemData.h"
#include "../private/PhysicsEntity.h"
#include "../private/CharacterController.h"
#include "../private/Util.h"

namespace sge
{
    namespace bullet_physics
	{
        PhysicsEntity& BulletPhysicsSystem::Data::get_or_create_physics_entity(NodeId node)
        {
            // Search for the entity
            auto iter = physics_entities.find(node);
            if (iter != physics_entities.end())
            {
                // Return the existing physics entity
                return *iter->second;
            }

            // Create a new physics entity
            auto phys = std::make_unique<PhysicsEntity>(node, *this);
			auto* phys_entity_ptr = phys.get();
            physics_entities.insert(std::make_pair(node, std::move(phys)));

			return *phys_entity_ptr;
        }

        PhysicsEntity* BulletPhysicsSystem::Data::get_physics_entity(NodeId node)
        {
            auto iter = physics_entities.find(node);
            return iter == physics_entities.end() ? nullptr : iter->second.get();
        }

	    void BulletPhysicsSystem::Data::post_add_physics_entity_element(PhysicsEntity& phys_entity)
	    {
			// If the physics entity does not have a ghost object, rigid body, or character controller, give it a basic collision object
			if (phys_entity.ghost_object == nullptr && phys_entity.rigid_body == nullptr && phys_entity.character_controller == nullptr)
			{
				if (phys_entity.collision_object != nullptr)
				{
					return;
				}

				// Create the collision object
				phys_entity.collision_object = std::make_unique<btCollisionObject>();
				phys_entity.collision_object->setCollisionShape(&phys_entity.collider);
				phys_entity.collision_object->setWorldTransform(phys_entity.transform);
				phys_entity.collision_object->setInterpolationWorldTransform(phys_entity.transform);

				// Add it to the world
				phys_world.dynamics_world().addCollisionObject(phys_entity.collision_object.get());
			}
			else
			{
				if (phys_entity.collision_object == nullptr)
				{
					return;
				}

				// Remove the collision object from the world, and destroy it
				phys_world.dynamics_world().removeCollisionObject(phys_entity.collision_object.get());
				phys_entity.collision_object = nullptr;
			}
	    }

	    void BulletPhysicsSystem::Data::post_remove_physics_entity_element(PhysicsEntity& phys_entity)
        {
            // If the physics entity has no rigid body, no ghost object, and no character controller
            if (phys_entity.rigid_body == nullptr &&
				phys_entity.ghost_object == nullptr &&
				phys_entity.character_controller == nullptr)
            {
				// If it has child shapes, then we can't remove it (evaluate if we need to add a collision object)
				if (phys_entity.collider.getNumChildShapes() != 0)
				{
					if (phys_entity.collision_object != nullptr)
					{
						return;
					}

					// Create a collision object, and add it to the world
					phys_entity.collision_object = std::make_unique<btCollisionObject>();
					phys_entity.collision_object->setCollisionShape(&phys_entity.collider);
					phys_entity.collision_object->setWorldTransform(phys_entity.transform);
					phys_entity.collision_object->setInterpolationWorldTransform(phys_entity.transform);
					phys_world.dynamics_world().addCollisionObject(phys_entity.collision_object.get());
					return;
				}

                // Remove the collision object, if any
				if (phys_entity.collision_object != nullptr)
				{
					phys_world.dynamics_world().removeCollisionObject(phys_entity.collision_object.get());
				}

				// Destroy it
                auto iter = physics_entities.find(phys_entity.node);
                physics_entities.erase(iter);
            }
        }
    }
}
