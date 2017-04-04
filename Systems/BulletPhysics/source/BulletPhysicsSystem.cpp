// BulletPhysicsSystem.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Engine/UpdatePipeline.h>
#include <Engine/Components/Physics/CBoxCollider.h>
#include <Engine/Components/Physics/CCapsuleCollider.h>
#include <Engine/Components/Physics/CRigidBody.h>
#include <Engine/Components/Gameplay/CCharacterController.h>
#include <Engine/SystemFrame.h>
#include <Engine/Scene.h>
#include <Engine/Tags/DebugDraw.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"
#include "../private/BulletPhysicsSystemData.h"
#include "../private/PhysicsEntity.h"
#include "../private/DebugDrawer.h"
#include "../private/CharacterController.h"
#include "../private/RigidBody.h"
#include "../private/Colliders.h"
#include "../private/Util.h"

SGE_REFLECT_TYPE(sge::bullet_physics::BulletPhysicsSystem);

namespace sge
{
    namespace bullet_physics
    {
		static void on_transform_modified(
			EventChannel& modified_transform_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			ENodeTransformChanged events[8];
			int32 num_events;
			while (modified_transform_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					// Get the physics state for this transform
					auto* phys_ent = phys_data.get_physics_entity(events[i].node->get_id());
					if (!phys_ent)
					{
						continue;
					}

					// Create the transform for the entity
					const btVector3 pos = to_bullet(events[i].node->get_local_position());
					const btQuaternion rot = to_bullet(events[i].node->get_local_rotation());
					btTransform trans;
					trans.setOrigin(pos);
					trans.setRotation(rot);
					phys_ent->extern_set_transform(trans);
				}
			}
		}

		static void on_character_controller_jump(
			EventChannel& jump_event_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			CCharacterController::EJump events[8];
			int32 num_events;
			while (jump_event_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					// Get the physics state for this node
					auto* phys_ent = phys_data.get_physics_entity(events[i].node);
					if (!phys_ent || !phys_ent->character_controller)
					{
						continue;
					}

					// Make it jump
					phys_ent->character_controller->jump(btVector3{ 0.0, 0.0, 0.0 });
				}
			}
		}

		static void on_character_controller_turn(
			EventChannel& turn_event_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			CCharacterController::ETurn events[8];
			int32 num_events;
			while (turn_event_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					// Get the physics state for this node
					auto* const phys_ent = phys_data.get_physics_entity(events[i].node);
					if (!phys_ent || !phys_ent->character_controller)
					{
						continue;
					}

					// Make it turn
					phys_ent->character_controller->turn(events[i].amount);
				}
			}
		}

		static void on_character_controller_walk(
			EventChannel& walk_event_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data)
		{
			// Get events
			CCharacterController::EWalk events[8];
			int32 num_events;
			while (walk_event_channel.consume(subscriber_id, events, &num_events))
			{
				for (int32 i = 0; i < num_events; ++i)
				{
					// Get the physics state for this node
					auto* const phys_ent = phys_data.get_physics_entity(events[i].node);
					if (!phys_ent || !phys_ent->character_controller)
					{
						continue;
					}

					// Make it walk
					phys_ent->character_controller->walk(events[i].direction);
				}
			}
		}

		static void update_scene_nodes(
			Node* const* nodes,
			const PhysTransformedNode* transforms,
			std::size_t num_transforms)
		{
			for (std::size_t i = 0; i < num_transforms; ++i)
			{
				nodes[i]->set_local_position(transforms[i].world_transform);
				nodes[i]->set_local_rotation(transforms[i].world_rotation);
			}
		}

        BulletPhysicsSystem::BulletPhysicsSystem(const Config& config)
            : _initialized_world(false),
			_node_world_transform_changed_event_channel(nullptr),
			_new_rigid_body_event_channel(nullptr),
			_modified_rigid_body_event_channel(nullptr),
			_destroyed_rigid_body_event_channel(nullptr),
			_new_box_collider_channel(nullptr),
			_modified_box_collider_channel(nullptr),
			_destroyed_box_collider_channel(nullptr),
			_new_capsule_collider_channel(nullptr),
			_modified_capsule_collider_channel(nullptr),
			_destroyed_capsule_collider_channel(nullptr),
			_character_controller_jump_event_channel(nullptr),
			_character_controller_turn_event_channel(nullptr),
			_character_controller_walk_event_channel(nullptr),
			_node_world_transform_changed_sid(EventChannel::INVALID_SID),
			_new_rigid_body_sid(EventChannel::INVALID_SID),
			_modified_rigid_body_sid(EventChannel::INVALID_SID),
			_destroyed_rigid_body_sid(EventChannel::INVALID_SID),
			_new_box_collider_sid(EventChannel::INVALID_SID),
			_modified_box_collider_sid(EventChannel::INVALID_SID),
			_destroyed_box_collider_sid(EventChannel::INVALID_SID),
			_new_capsule_collider_sid(EventChannel::INVALID_SID),
    		_modified_capsule_collider_sid(EventChannel::INVALID_SID),
			_destroyed_capsule_collider_sid(EventChannel::INVALID_SID),
    		_character_controller_jump_sid(EventChannel::INVALID_SID),
			_character_controller_turn_sid(EventChannel::INVALID_SID),
			_character_controller_walk_sid(EventChannel::INVALID_SID)
        {
            _data = std::make_unique<Data>();
        }

        BulletPhysicsSystem::~BulletPhysicsSystem()
        {
        }

        void BulletPhysicsSystem::register_pipeline(UpdatePipeline& pipeline)
        {
            pipeline.register_system_fn(
                "bullet_physics",
                this,
                &BulletPhysicsSystem::phys_tick);

            pipeline.register_system_fn(
                "bullet_physics_debug_draw",
                this,
                &BulletPhysicsSystem::debug_draw);
        }

	    void BulletPhysicsSystem::initialize_subscriptions(Scene& scene)
	    {
			_node_world_transform_changed_event_channel = scene.get_node_world_transform_changed_channel();
			_node_world_transform_changed_sid = _node_world_transform_changed_event_channel->subscribe();

			// Rigid Body event subscriptions
			_new_rigid_body_event_channel = scene.get_event_channel(CRigidBody::type_info, "new");
			_modified_rigid_body_event_channel = scene.get_event_channel(CRigidBody::type_info, "prop_mod");
			_destroyed_rigid_body_event_channel = scene.get_event_channel(CRigidBody::type_info, "destroy");
			_new_rigid_body_sid = _new_rigid_body_event_channel->subscribe();
			_modified_rigid_body_sid = _modified_rigid_body_event_channel->subscribe();
			_destroyed_rigid_body_sid = _destroyed_rigid_body_event_channel->subscribe();

			// Box collider event subscriptions
			_new_box_collider_channel = scene.get_event_channel(CBoxCollider::type_info, "new");
			_modified_box_collider_channel = scene.get_event_channel(CBoxCollider::type_info, "prop_mod");
			_destroyed_box_collider_channel = scene.get_event_channel(CBoxCollider::type_info, "destroy");
			_new_box_collider_sid = _new_box_collider_channel->subscribe();
			_modified_box_collider_sid = _modified_box_collider_channel->subscribe();
			_destroyed_box_collider_sid = _destroyed_box_collider_channel->subscribe();

			// Capsule collider event subscriptions
			_new_capsule_collider_channel = scene.get_event_channel(CCapsuleCollider::type_info, "new");
			_modified_capsule_collider_channel = scene.get_event_channel(CCapsuleCollider::type_info, "prop_mod");
			_destroyed_capsule_collider_channel = scene.get_event_channel(CCapsuleCollider::type_info, "destroy");
			_new_capsule_collider_sid = _new_capsule_collider_channel->subscribe();
			_modified_capsule_collider_sid = _modified_capsule_collider_channel->subscribe();
			_destroyed_capsule_collider_sid = _destroyed_capsule_collider_channel->subscribe();

			// Character controller event subscriptions
			_character_controller_jump_event_channel = scene.get_event_channel(CCharacterController::type_info, "jump_channel");
			_character_controller_turn_event_channel = scene.get_event_channel(CCharacterController::type_info, "turn_channel");
			_character_controller_walk_event_channel = scene.get_event_channel(CCharacterController::type_info, "walk_channel");
			_character_controller_jump_sid = _character_controller_jump_event_channel->subscribe();
			_character_controller_turn_sid = _character_controller_turn_event_channel->subscribe();
			_character_controller_walk_sid = _character_controller_walk_event_channel->subscribe();
	    }

	    void BulletPhysicsSystem::phys_tick(Scene& scene, SystemFrame& frame)
        {
			// Consume events
			on_transform_modified(*_node_world_transform_changed_event_channel, _node_world_transform_changed_sid, *_data);

			// Consume rigid body events
			on_new_rigid_body(scene, *_new_rigid_body_event_channel, _new_rigid_body_sid, *_data);
			on_rigid_body_modified(*_modified_rigid_body_event_channel, _modified_rigid_body_sid, *_data);
			on_destroy_rigid_body(*_destroyed_rigid_body_event_channel, _destroyed_rigid_body_sid, *_data);

			// Consume box collider events
			on_box_collider_new(*_new_box_collider_channel, _new_box_collider_sid, *_data);
			on_box_collider_modified(*_modified_box_collider_channel, _modified_box_collider_sid, *_data);
			on_box_collider_destroyed(*_destroyed_box_collider_channel, _destroyed_box_collider_sid, *_data);

			// Consume capsule collider events
			on_capsule_collider_new(*_new_capsule_collider_channel, _new_capsule_collider_sid, *_data);
			on_capsule_collider_modified(*_modified_capsule_collider_channel, _modified_box_collider_sid, *_data);
			on_capsule_collider_destroyed(*_destroyed_capsule_collider_channel, _destroyed_capsule_collider_sid, *_data);

			// Initialize the world, if we haven't already
			if (!_initialized_world)
			{
				_initialized_world = true;
				initialize_world(scene);
			}

			// Consume character controller events
			on_character_controller_jump(*_character_controller_jump_event_channel, _character_controller_jump_sid, *_data);
			on_character_controller_turn(*_character_controller_turn_event_channel, _character_controller_turn_sid, *_data);
			on_character_controller_walk(*_character_controller_walk_event_channel, _character_controller_walk_sid, *_data);

            // Simulate physics
            _data->phys_world.dynamics_world().stepSimulation(frame.time_delta(), 3);

			// Update scene transforms
			const std::size_t num_transforms = _data->frame_transformed_nodes.size();
			auto* const scene_nodes = (Node**)std::malloc(num_transforms * sizeof(Node*));
			scene.get_nodes(_data->frame_transformed_nodes.data(), num_transforms, scene_nodes);
			update_scene_nodes(scene_nodes, _data->frame_transformed_node_transforms.data(), num_transforms);
			std::free(scene_nodes);

			// Acknowledge transform events (so we don't get stuck in a feedback loop)
			frame.yield();
			_node_world_transform_changed_event_channel->acknowledge_unconsumed(_node_world_transform_changed_sid);

			// Clear data
            _data->frame_transformed_nodes.clear();
			_data->frame_transformed_node_transforms.clear();
        }

        void BulletPhysicsSystem::debug_draw(Scene& scene, SystemFrame& frame)
        {
            DebugDrawer drawer;

            // Draw the world
            _data->phys_world.dynamics_world().setDebugDrawer(&drawer);
            _data->phys_world.dynamics_world().debugDrawWorld();
            _data->phys_world.dynamics_world().setDebugDrawer(nullptr);

            // Create a tag buffer

        }

        void BulletPhysicsSystem::initialize_world(Scene& scene)
        {
			auto* character_controller_component = scene.get_component_container(CCharacterController::type_info);

			// Load all character controllers
			{
				NodeId character_controller_nodes[8];
				std::size_t start_index = 0;
				std::size_t num_instances;
				while (character_controller_component->get_instance_nodes(start_index, 8, &num_instances, character_controller_nodes))
				{
					start_index += 8;
					CCharacterController* character_controller_instances[8];
					const Node* node_instances[8];
					character_controller_component->get_instances(character_controller_nodes, num_instances, character_controller_instances);
					scene.get_nodes(character_controller_nodes, num_instances, node_instances);

					for (std::size_t i = 0; i < num_instances; ++i)
					{
						_data->add_character_controller(*node_instances[i], *character_controller_instances[i]);
					}
				}
			}
        }
    }
}
