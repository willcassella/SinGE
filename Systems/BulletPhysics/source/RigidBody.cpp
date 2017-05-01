// RigidBody.cpp

#include <Engine/Scene.h>
#include <Engine/Components/Physics/CRigidBody.h>
#include "../private/RigidBody.h"
#include "../private/PhysicsEntity.h"
#include "../private/Util.h"

namespace sge
{
	void bullet_physics::on_rigid_body_new(
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
				auto& physics_entity = phys_data.get_or_create_physics_entity(node_ids[i], *nodes[i]);
				assert(physics_entity.rigid_body == nullptr);

				// Calculate local inertia from the collision shape
				btScalar mass = 0.f;
				btVector3 local_inertia{ 0.f, 0.f, 0.f };
				if (!instances[i]->kinematic())
				{
					mass = instances[i]->mass();
					physics_entity.collider.calculateLocalInertia(mass, local_inertia);
				}

				// Create the rigid body construction info
				btRigidBody::btRigidBodyConstructionInfo cinfo{
					mass,
					&physics_entity,
					&physics_entity.collider,
					local_inertia };
				cinfo.m_friction = instances[i]->friction();
				cinfo.m_rollingFriction = instances[i]->rolling_friction();
				cinfo.m_spinningFriction = instances[i]->spinning_friction();
				cinfo.m_linearDamping = instances[i]->linear_damping();
				cinfo.m_angularDamping = instances[i]->angular_damping();

				// Create the rigid body
				physics_entity.rigid_body = std::make_unique<btRigidBody>(cinfo);
				physics_entity.rigid_body->setUserIndex(physics_entity.get_user_index_1());
				physics_entity.rigid_body->setUserIndex2(physics_entity.get_user_index_2());
				physics_entity.rigid_body->setUserPointer(&physics_entity);

				// Set kinematic flags
				if (instances[i]->kinematic())
				{
					physics_entity.rigid_body->setActivationState(DISABLE_DEACTIVATION);
					physics_entity.rigid_body->setCollisionFlags(
						physics_entity.rigid_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
				}

				// Add it to the world
				phys_data.phys_world.dynamics_world().addRigidBody(physics_entity.rigid_body.get());

				// Remove collision object if necessary
				phys_data.post_add_physics_entity_element(physics_entity);
			}
		}
	}

	void bullet_physics::on_rigid_body_destroyed(
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
				const auto node = events[i].node;

				auto* physics_entity = phys_data.get_physics_entity(node);
				assert(physics_entity != nullptr && physics_entity->rigid_body != nullptr);

				// Remove the rigid body from the world and delete it
				phys_data.phys_world.dynamics_world().removeRigidBody(physics_entity->rigid_body.get());
				physics_entity->rigid_body = nullptr;

				// Evaluate if we should keep the physics entity or not
				phys_data.post_remove_physics_entity_element(*physics_entity);
			}
		}
	}

	void bullet_physics::on_rigid_body_modified(
		EventChannel& modified_rigid_body_channel,
		EventChannel::SubscriberId subscriber_id,
		BulletPhysicsSystem::Data& phys_data)
	{
		// Get events
		EModifiedComponent events[8];
		int32 num_events;
		while (modified_rigid_body_channel.consume(subscriber_id, events, &num_events))
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
