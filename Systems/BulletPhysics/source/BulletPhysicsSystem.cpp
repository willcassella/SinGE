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
			_node_transform_event_channel(nullptr),
			_character_controller_jump_event_channel(nullptr),
			_character_controller_turn_event_channel(nullptr),
			_character_controller_walk_event_channel(nullptr),
			_node_transform_changed_sid(0),
			_character_controller_jump_sid(0),
			_character_controller_turn_sid(0),
			_character_controller_walk_sid(0)
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

        void BulletPhysicsSystem::phys_tick(Scene& scene, SystemFrame& frame)
        {
            // Initialize the world, if we haven't already
            if (!_initialized_world)
            {
                _initialized_world = true;
                initialize_world(scene);
				_node_transform_event_channel = scene.get_node_transform_changed_channel();
				_node_transform_changed_sid = _node_transform_event_channel->subscribe();
				_character_controller_jump_event_channel = scene.get_event_channel(CCharacterController::type_info, "jump_channel");
				_character_controller_jump_sid = _character_controller_jump_event_channel->subscribe();
				_character_controller_turn_event_channel = scene.get_event_channel(CCharacterController::type_info, "turn_channel");
				_character_controller_turn_sid = _character_controller_turn_event_channel->subscribe();
				_character_controller_walk_event_channel = scene.get_event_channel(CCharacterController::type_info, "walk_channel");
				_character_controller_walk_sid = _character_controller_walk_event_channel->subscribe();
            }

			// Consume events
			on_transform_modified(*_node_transform_event_channel, _node_transform_changed_sid, *_data);
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
			_node_transform_event_channel->acknowledge_unconsumed(_node_transform_changed_sid);

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
			auto* box_collider_component = scene.get_component_container(CBoxCollider::type_info);
			auto* capsule_collider_component = scene.get_component_container(CCapsuleCollider::type_info);
			auto* rigid_body_component = scene.get_component_container(CRigidBody::type_info);
			auto* character_controller_component = scene.get_component_container(CCharacterController::type_info);

			// Load all box colliders
			{
				NodeId box_collider_nodes[8];
				std::size_t start_index = 0;
				std::size_t num_instances;
				while (box_collider_component->get_instance_nodes(start_index, 8, &num_instances, box_collider_nodes))
				{
					start_index += 8;
					CBoxCollider* box_collider_instances[8];
					box_collider_component->get_instances(box_collider_nodes, num_instances, box_collider_instances);

					for (std::size_t i = 0; i < num_instances; ++i)
					{
						_data->add_box_collider(box_collider_nodes[i], *box_collider_instances[i]);
					}
				}
			}

            // Load all capsule collider
			{
				NodeId capsule_collider_nodes[8];
				std::size_t start_index = 0;
				std::size_t num_instances;
				while (capsule_collider_component->get_instance_nodes(start_index, 8, &num_instances, capsule_collider_nodes))
				{
					start_index += 8;
					CCapsuleCollider* capsule_collider_instances[8];
					capsule_collider_component->get_instances(capsule_collider_nodes, num_instances, capsule_collider_instances);

					for (std::size_t i = 0; i < num_instances; ++i)
					{
						_data->add_capsule_collider(capsule_collider_nodes[i], *capsule_collider_instances[i]);
					}
				}
			}

			// Load all rigid bodies
			{
				NodeId rigid_body_nodes[8];
				std::size_t start_index = 0;
				std::size_t num_instances;
				while (rigid_body_component->get_instance_nodes(start_index, 8, &num_instances, rigid_body_nodes))
				{
					start_index += 8;
					CRigidBody* rigid_body_instances[8];
					const Node* node_instances[8];
					rigid_body_component->get_instances(rigid_body_nodes, num_instances, rigid_body_instances);
					scene.get_nodes(rigid_body_nodes, num_instances, node_instances);

					for (std::size_t i = 0; i < num_instances; ++i)
					{
						_data->add_rigid_body(*node_instances[8], *rigid_body_instances[i]);
					}
				}
			}

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
