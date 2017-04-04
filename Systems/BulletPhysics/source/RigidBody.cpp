// RigidBody.cpp

#include <Engine/Scene.h>
#include <Engine/Components/Physics/CRigidBody.h>
#include "../private/RigidBody.h"
#include "../private/PhysicsEntity.h"

namespace sge
{
	void bullet_physics::on_new_rigid_body(
		Scene& scene,
		EventChannel& new_rigid_body_channel,
		EventChannel::SubscriberId subscriber_id,
		BulletPhysicsSystem::Data& phys_data)
	{
		// Get events
		ENewComponent events[8];
		int32 num_events;
		while (new_rigid_body_channel.consume(subscriber_id, events, &num_events))
		{
			NodeId node_ids[8];
			const CRigidBody* instances[8];
			for (int32 i = 0; i < num_events; ++i)
			{
				node_ids[i] = events[i].node;
				instances[i] = (const CRigidBody*)events[i].instance;
			}

			// Get nodes
			const Node* nodes[8];
			scene.get_nodes(node_ids, num_events, nodes);

			// Create rigid bodies
			for (int32 i = 0; i < num_events; ++i)
			{
				phys_data.add_rigid_body(*nodes[i], *instances[i]);
			}
		}
	}

	void bullet_physics::on_destroy_rigid_body(
		EventChannel& destroyed_rigid_body_channel,
		EventChannel::SubscriberId subscriber_id,
		BulletPhysicsSystem::Data& phys_data)
	{
		// Get events
		EDestroyedComponent events[8];
		int32 num_events;
		while (destroyed_rigid_body_channel.consume(subscriber_id, events, &num_events))
		{
			for (int32 i = 0; i < num_events; ++i)
			{
				phys_data.remove_rigid_body(events[i].node);
			}
		}
	}

	void bullet_physics::on_rigid_body_modified(
		EventChannel& rigid_body_modified_channel,
		EventChannel::SubscriberId subscriber_id,
		BulletPhysicsSystem::Data& phys_data)
	{
		// Get events
		EModifiedComponent events[8];
		int32 num_events;
		while (rigid_body_modified_channel.consume(subscriber_id, events, &num_events))
		{
			// For each event
			for (int32 i = 0; i < num_events; ++i)
			{
				auto& event = events[i];
				const auto* const instance = (const CRigidBody*)event.instance;

				// Get the physics entity associated with this event
				auto* const phys_ent = phys_data.get_physics_entity(event.node);
				assert(phys_ent != nullptr);
				assert(phys_ent->rigid_body != nullptr);
				btRigidBody* rigid_body = phys_ent->rigid_body.get();

				// Set the properties on the rigid body
				if (event.property == "kinematic")
				{
					if (instance->kinematic())
					{
						rigid_body->setMassProps(0, { 0, 0, 0 });
						rigid_body->setActivationState(DISABLE_DEACTIVATION);
						rigid_body->setCollisionFlags(rigid_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
					}
					else
					{
						// Calculate local inertia and mass
						btVector3 local_inertia;
						phys_ent->collider.calculateLocalInertia(instance->mass(), local_inertia);
						rigid_body->setMassProps(instance->mass(), local_inertia);

						// Set other rigid body properties
						rigid_body->setActivationState(ACTIVE_TAG);
						rigid_body->setCollisionFlags(rigid_body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
					}
				}
				else
				{
					// Set misc properties
					rigid_body->setFriction(instance->friction());
					rigid_body->setRollingFriction(instance->rolling_friction());
					rigid_body->setSpinningFriction(instance->spinning_friction());
					rigid_body->setDamping(instance->linear_damping(), instance->angular_damping());
				}
			}
		}
	}
}
