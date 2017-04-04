// Colliders.cpp

#include <Engine/Components/Physics/CSphereCollider.h>
#include <Engine/Components/Physics/CBoxCollider.h>
#include <Engine/Components/Physics/CCapsuleCollider.h>
#include "../private/Colliders.h"
#include "../private/PhysicsEntity.h"
#include "../private/Util.h"

namespace sge
{
	namespace bullet_physics
	{
		void on_sphere_collider_new(
			EventChannel& new_sphere_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			ENewComponent events[8];
			int32 num_events;
			while (new_sphere_collider_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					NodeId node = events[i].node;
					const auto* const component = (const CSphereCollider*)events[i].instance;

					// Add the sphere collider
					auto& physics_entity = phys_data.get_or_create_physics_entity(node);
					assert(physics_entity.sphere_collider == nullptr);
					physics_entity.sphere_collider = std::make_unique<btSphereShape>(component->radius());
					physics_entity.collider.addChildShape(btTransform::getIdentity(), physics_entity.sphere_collider.get());
				}
			}
		}

		void on_sphere_collider_destroyed(
			EventChannel& destroyed_sphere_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			EDestroyedComponent events[8];
			int32 num_events;
			while (destroyed_sphere_collider_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					NodeId node = events[i].node;

					// Destroy the sphere collider
					auto* const physics_entity = phys_data.get_physics_entity(node);
					assert(physics_entity != nullptr && physics_entity->sphere_collider != nullptr);
					physics_entity->collider.removeChildShape(physics_entity->sphere_collider.get());
					physics_entity->sphere_collider = nullptr;

					// Evaluate if we still need this physics entity
					phys_data.post_remove_physics_entity_element(*physics_entity);
				}
			}
		}

		void on_sphere_collider_modified(
			EventChannel& modified_sphere_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			EModifiedComponent events[8];
			int32 num_events;
			while (modified_sphere_collider_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					const NodeId node = events[i].node;
					const auto* const component = (const CSphereCollider*)events[i].instance;

					auto* const physics_entity = phys_data.get_physics_entity(node);
					assert(physics_entity != nullptr && physics_entity->sphere_collider != nullptr);

					// Update the sphere collider by destroying it in-place and creating a new one
					physics_entity->collider.removeChildShape(physics_entity->sphere_collider.get());
					physics_entity->sphere_collider->~btSphereShape();
					new (physics_entity->sphere_collider.get()) btSphereShape(component->radius());
					physics_entity->collider.addChildShape(btTransform::getIdentity(), physics_entity->sphere_collider.get());
				}
			}
		}

		void on_box_collider_new(
			EventChannel& new_box_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			ENewComponent events[8];
			int32 num_events;
			while (new_box_collider_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					NodeId node = events[i].node;
					const auto* const component = (const CBoxCollider*)events[i].instance;

					// Add the box collider
					auto& physics_entity = phys_data.get_or_create_physics_entity(node);
					assert(physics_entity.box_collider == nullptr);
					physics_entity.box_collider = std::make_unique<btBoxShape>(to_bullet(component->shape() / 2));
					physics_entity.collider.addChildShape(btTransform::getIdentity(), physics_entity.box_collider.get());
				}
			}
		}

		void on_box_collider_destroyed(
			EventChannel& destroyed_box_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			EDestroyedComponent events[8];
			int32 num_events;
			while (destroyed_box_collider_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					NodeId node = events[i].node;

					auto* phys_entity = phys_data.get_physics_entity(node);
					assert(phys_entity != nullptr && phys_entity->box_collider != nullptr);

					// Remove the box collider from the compound and the physisc entity
					phys_entity->collider.removeChildShape(phys_entity->box_collider.get());
					phys_entity->box_collider = nullptr;

					// Evaluate if the physics entity needs to stay in the physics world
					phys_data.post_remove_physics_entity_element(*phys_entity);
				}
			}

		}

		void on_box_collider_modified(
			EventChannel& modified_box_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			EModifiedComponent events[8];
			int32 num_events;
			while (modified_box_collider_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					NodeId node = events[i].node;
					const auto* const component = (const CBoxCollider*)events[i].instance;

					auto* phys_entity = phys_data.get_physics_entity(node);
					assert(phys_entity != nullptr && phys_entity->box_collider != nullptr);

					// Update the box collider (destroy it in place, and reconstruct it with the new shape)
					const btVector3 shape = to_bullet(component->shape() / 2);
					phys_entity->collider.removeChildShape(phys_entity->box_collider.get());
					phys_entity->box_collider->~btBoxShape();
					new (phys_entity->box_collider.get()) btBoxShape(shape);
					phys_entity->collider.addChildShape(btTransform::getIdentity(), phys_entity->box_collider.get());
				}
			}
		}

		void on_capsule_collider_new(
			EventChannel& new_capsule_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			ENewComponent events[8];
			int32 num_events;
			while (new_capsule_collider_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					NodeId node = events[i].node;
					const auto* const component = (CCapsuleCollider*)events[i].instance;

					auto& phys_entity = phys_data.get_or_create_physics_entity(node);
					assert(phys_entity.capsule_collider == nullptr);

					// Create a new capsule collider
					phys_entity.capsule_collider = std::make_unique<btCapsuleShape>(component->radius(), component->height());
					phys_entity.collider.addChildShape(btTransform::getIdentity(), phys_entity.capsule_collider.get());
				}
			}
		}

		void on_capsule_collider_destroyed(
			EventChannel& destroyed_capsule_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			EDestroyedComponent events[8];
			int32 num_events;
			while (destroyed_capsule_collider_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					NodeId node = events[i].node;

					auto* physics_entity = phys_data.get_physics_entity(node);
					assert(physics_entity != nullptr && physics_entity->capsule_collider != nullptr);

					// Remove the capsule colider from the compound and the physics entity
					physics_entity->collider.removeChildShape(physics_entity->capsule_collider.get());
					physics_entity->capsule_collider = nullptr;

					// Evaluate if this physics entity should stay in the scene
					phys_data.post_remove_physics_entity_element(*physics_entity);
				}
			}
		}

		void on_capsule_collider_modified(
			EventChannel& modified_capsule_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Gete events
			EModifiedComponent events[8];
			int32 num_events;
			while (modified_capsule_collider_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					NodeId node = events[i].node;
					const auto* const component = (const CCapsuleCollider*)events[i].instance;

					auto* physics_entity = phys_data.get_physics_entity(node);
					assert(physics_entity != nullptr && physics_entity->capsule_collider != nullptr);

					// Update the capsule collider by destroying it in-place and creating a new one
					physics_entity->collider.removeChildShape(physics_entity->capsule_collider.get());
					physics_entity->capsule_collider->~btCapsuleShape();
					new (physics_entity->capsule_collider.get()) btCapsuleShape(component->radius(), component->height());
					physics_entity->collider.addChildShape(btTransform::getIdentity(), physics_entity->capsule_collider.get());
				}
			}
		}
	}
}
