// LevelPortal.cpp

#include <Engine/Scene.h>
#include "../private/LevelPortal.h"
#include "../private/PhysicsEntity.h"
#include "../private/BulletPhysicsSystemData.h"

namespace sge
{
	namespace bullet_physics
	{
		void on_level_portal_new(
			EventChannel& new_level_portal_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data,
			Scene& scene)
		{
			// Get events
			ENewComponent events[8];
			int32 num_events;
			while (new_level_portal_channel.consume(subscriber_id, events, &num_events))
			{
				// Get nodes
				NodeId node_ids[8];
				for (int32 i = 0; i < num_events; ++i)
				{
					node_ids[i] = events[i].node;
				}

				const Node* nodes[8];
				scene.get_nodes(node_ids, num_events, nodes);

				// Process events
				for (int32 i = 0; i < num_events; ++i)
				{
					auto& phys_entity = phys_data.get_or_create_physics_entity(node_ids[i], *nodes[i]);

					// Create a ghost object for it
					if (!phys_entity.ghost_object)
					{
						phys_entity.ghost_object = std::make_unique<btPairCachingGhostObject>();
						phys_entity.ghost_object->setCollisionShape(&phys_entity.collider);
						phys_entity.ghost_object->setWorldTransform(phys_entity.transform);
						phys_entity.ghost_object->setInterpolationWorldTransform(phys_entity.transform);
						phys_entity.ghost_object->setUserPointer(&phys_entity);
						phys_entity.ghost_object->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
						phys_data.phys_world.dynamics_world().addCollisionObject(phys_entity.ghost_object.get());
						phys_data.post_add_physics_entity_element(phys_entity);
					}

					phys_entity.set_user_index_1(phys_entity.get_user_index_1() | LEVEL_PORTAL_BIT);
				}
			}
		}

		void on_level_portal_destroyed(
			EventChannel& destroyed_level_portal_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			EDestroyedComponent events[8];
			int32 num_events;
			while (destroyed_level_portal_channel.consume(subscriber_id, events, &num_events))
			{
				// Process events
				for (int32 i = 0; i < num_events; ++i)
				{
					auto* phys_entity = phys_data.get_physics_entity(events[i].node);
					if (!phys_entity)
					{
						continue;
					}

					// Remove the ghost object
					if (phys_entity->ghost_object)
					{
						phys_data.phys_world.dynamics_world().removeCollisionObject(phys_entity->ghost_object.get());
						phys_entity->ghost_object = nullptr;
					}

					phys_entity->set_user_index_1(phys_entity->get_user_index_1() & ~LEVEL_PORTAL_BIT);
					phys_data.post_remove_physics_entity_element(*phys_entity);
				}
			}
		}
	}
}
