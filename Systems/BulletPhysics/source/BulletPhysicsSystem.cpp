// BulletPhysicsSystem.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Engine/UpdatePipeline.h>
#include <Engine/Components/Physics/CSphereCollider.h>
#include <Engine/Components/Physics/CBoxCollider.h>
#include <Engine/Components/Physics/CCapsuleCollider.h>
#include <Engine/Components/Physics/CStaticMeshCollider.h>
#include <Engine/Components/Physics/CRigidBody.h>
#include <Engine/Components/Gameplay/CCharacterController.h>
#include <Engine/Components/Gameplay/CLevelPortal.h>
#include <Engine/SystemFrame.h>
#include <Engine/Scene.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"
#include "../private/BulletPhysicsSystemData.h"
#include "../private/PhysicsEntity.h"
#include "../private/DebugDrawer.h"
#include "../private/CharacterController.h"
#include "../private/RigidBody.h"
#include "../private/Colliders.h"
#include "../private/LevelPortal.h"
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
					btTransform trans;
					const btVector3 pos = to_bullet(events[i].node->get_local_position());
					const btVector3 scale = to_bullet(events[i].node->get_local_scale());
					const btQuaternion rot = to_bullet(events[i].node->get_local_rotation());
					trans.setOrigin(pos);
					trans.setRotation(rot);
					phys_ent->extern_set_transform(trans, scale);
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

        BulletPhysicsSystem::BulletPhysicsSystem(const Config& /*config*/)
            : _node_world_transform_changed_channel(nullptr),
			_new_rigid_body_channel(nullptr),
			_modified_rigid_body_channel(nullptr),
			_destroyed_rigid_body_channel(nullptr),
			_new_sphere_collider_channel(nullptr),
			_modified_sphere_collider_channel(nullptr),
			_destroyed_sphere_collider_channel(nullptr),
			_new_box_collider_channel(nullptr),
			_modified_box_collider_channel(nullptr),
			_destroyed_box_collider_channel(nullptr),
			_new_capsule_collider_channel(nullptr),
			_modified_capsule_collider_channel(nullptr),
			_destroyed_capsule_collider_channel(nullptr),
			_new_static_mesh_collider_channel(nullptr),
			_modified_static_mesh_collider_channel(nullptr),
			_destroyed_static_mesh_collider_channel(nullptr),
    		_new_character_controller_channel(nullptr),
			_modified_character_controller_channel(nullptr),
			_character_controller_jump_event_channel(nullptr),
			_character_controller_turn_event_channel(nullptr),
			_character_controller_walk_event_channel(nullptr),
			_destroyed_character_controller_channel(nullptr),
			_new_level_portal_channel(nullptr),
			_destroyed_level_portal_channel(nullptr),
			_node_world_transform_changed_sid(EventChannel::INVALID_SID),
			_new_rigid_body_sid(EventChannel::INVALID_SID),
			_modified_rigid_body_sid(EventChannel::INVALID_SID),
			_destroyed_rigid_body_sid(EventChannel::INVALID_SID),
			_new_sphere_collider_sid(EventChannel::INVALID_SID),
			_modified_sphere_collider_sid(EventChannel::INVALID_SID),
			_destroyed_sphere_collider_sid(EventChannel::INVALID_SID),
			_new_box_collider_sid(EventChannel::INVALID_SID),
			_modified_box_collider_sid(EventChannel::INVALID_SID),
			_destroyed_box_collider_sid(EventChannel::INVALID_SID),
			_new_capsule_collider_sid(EventChannel::INVALID_SID),
    		_modified_capsule_collider_sid(EventChannel::INVALID_SID),
			_destroyed_capsule_collider_sid(EventChannel::INVALID_SID),
			_new_static_mesh_collider_sid(EventChannel::INVALID_SID),
			_modified_static_mesh_collider_sid(EventChannel::INVALID_SID),
			_destroyed_static_mesh_collider_sid(EventChannel::INVALID_SID),
			_new_character_controller_sid(EventChannel::INVALID_SID),
			_modified_character_controller_sid(EventChannel::INVALID_SID),
    		_character_controller_jump_sid(EventChannel::INVALID_SID),
			_character_controller_turn_sid(EventChannel::INVALID_SID),
			_character_controller_walk_sid(EventChannel::INVALID_SID),
			_destroyed_character_controller_sid(EventChannel::INVALID_SID),
			_new_level_portal_sid(EventChannel::INVALID_SID),
			_destroyed_level_portal_sid(EventChannel::INVALID_SID)
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
			_node_world_transform_changed_channel = scene.get_node_world_transform_changed_channel();
			_node_world_transform_changed_sid = _node_world_transform_changed_channel->subscribe();

			// Rigid Body event subscriptions
			_new_rigid_body_channel = scene.get_event_channel(CRigidBody::type_info, "new");
			_modified_rigid_body_channel = scene.get_event_channel(CRigidBody::type_info, "prop_mod");
			_destroyed_rigid_body_channel = scene.get_event_channel(CRigidBody::type_info, "destroy");
			_new_rigid_body_sid = _new_rigid_body_channel->subscribe();
			_modified_rigid_body_sid = _modified_rigid_body_channel->subscribe();
			_destroyed_rigid_body_sid = _destroyed_rigid_body_channel->subscribe();

			// Sphere collider subscriptions
			_new_sphere_collider_channel = scene.get_event_channel(CSphereCollider::type_info, "new");
			_modified_sphere_collider_channel = scene.get_event_channel(CSphereCollider::type_info, "prop_mod");
			_destroyed_sphere_collider_channel = scene.get_event_channel(CSphereCollider::type_info, "destroy");
			_new_sphere_collider_sid = _new_sphere_collider_channel->subscribe();
			_modified_sphere_collider_sid = _modified_sphere_collider_channel->subscribe();
			_destroyed_sphere_collider_sid = _destroyed_sphere_collider_channel->subscribe();

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

			// Static mesh collider event subscriptions
			_new_static_mesh_collider_channel = scene.get_event_channel(CStaticMeshCollider::type_info, "new");
			_modified_static_mesh_collider_channel = scene.get_event_channel(CStaticMeshCollider::type_info, "prop_mod");
			_destroyed_static_mesh_collider_channel = scene.get_event_channel(CStaticMeshCollider::type_info, "destroy");
			_new_static_mesh_collider_sid = _new_static_mesh_collider_channel->subscribe();
			_modified_static_mesh_collider_sid = _modified_static_mesh_collider_channel->subscribe();
			_destroyed_static_mesh_collider_sid = _destroyed_static_mesh_collider_channel->subscribe();

			// Character controller event subscriptions
			_new_character_controller_channel = scene.get_event_channel(CCharacterController::type_info, "new");
			_modified_character_controller_channel = scene.get_event_channel(CCharacterController::type_info, "prop_mod");
			_character_controller_jump_event_channel = scene.get_event_channel(CCharacterController::type_info, "jump_channel");
			_character_controller_turn_event_channel = scene.get_event_channel(CCharacterController::type_info, "turn_channel");
			_character_controller_walk_event_channel = scene.get_event_channel(CCharacterController::type_info, "walk_channel");
			_destroyed_character_controller_channel = scene.get_event_channel(CCharacterController::type_info, "destroy");
			_new_character_controller_sid = _new_character_controller_channel->subscribe();
			_modified_character_controller_sid = _modified_character_controller_channel->subscribe();
			_character_controller_jump_sid = _character_controller_jump_event_channel->subscribe();
			_character_controller_turn_sid = _character_controller_turn_event_channel->subscribe();
			_character_controller_walk_sid = _character_controller_walk_event_channel->subscribe();
			_destroyed_character_controller_sid = _destroyed_character_controller_channel->subscribe();

			// Level portal event subscriptions
			_new_level_portal_channel = scene.get_event_channel(CLevelPortal::type_info, "new");
			_destroyed_level_portal_channel = scene.get_event_channel(CLevelPortal::type_info, "destroy");
			_new_level_portal_sid = _new_level_portal_channel->subscribe();
			_destroyed_level_portal_sid = _destroyed_level_portal_channel->subscribe();
	    }

	    void BulletPhysicsSystem::reset()
	    {
			auto& dynamics_world = _data->phys_world.dynamics_world();

			for (auto& phys_ent : _data->physics_entities)
			{
				if (phys_ent.second->collision_object)
				{
					dynamics_world.removeCollisionObject(phys_ent.second->collision_object.get());
				}
				if (phys_ent.second->ghost_object)
				{
					dynamics_world.removeCollisionObject(phys_ent.second->ghost_object.get());
				}
				if (phys_ent.second->rigid_body)
				{
					dynamics_world.removeRigidBody(phys_ent.second->rigid_body.get());
				}
				if (phys_ent.second->character_controller)
				{
					dynamics_world.removeAction(phys_ent.second->character_controller.get());
					dynamics_world.removeCollisionObject(&phys_ent.second->character_controller->ghost_object);
				}
			}

			_data->physics_entities.clear();
			_data->frame_transformed_nodes.clear();
			_data->frame_transformed_node_transforms.clear();
	    }

	    void BulletPhysicsSystem::consume_events(Scene& scene)
	    {
			// Update transforms
			on_transform_modified(*_node_world_transform_changed_channel, _node_world_transform_changed_sid, *_data);

			// Consume sphere collider events
			on_sphere_collider_new(*_new_sphere_collider_channel, _new_sphere_collider_sid, *_data, scene);
			on_sphere_collider_modified(*_modified_sphere_collider_channel, _modified_sphere_collider_sid, *_data);
			on_sphere_collider_destroyed(*_destroyed_sphere_collider_channel, _destroyed_sphere_collider_sid, *_data);

			// Consume box collider events
			on_box_collider_new(*_new_box_collider_channel, _new_box_collider_sid, *_data, scene);
			on_box_collider_modified(*_modified_box_collider_channel, _modified_box_collider_sid, *_data);
			on_box_collider_destroyed(*_destroyed_box_collider_channel, _destroyed_box_collider_sid, *_data);

			// Consume capsule collider events
			on_capsule_collider_new(*_new_capsule_collider_channel, _new_capsule_collider_sid, *_data, scene);
			on_capsule_collider_modified(*_modified_capsule_collider_channel, _modified_box_collider_sid, *_data);
			on_capsule_collider_destroyed(*_destroyed_capsule_collider_channel, _destroyed_capsule_collider_sid, *_data);

			// Consume static mesh collider events
			on_static_mesh_collider_new(*_new_static_mesh_collider_channel, _new_static_mesh_collider_sid, *_data, scene);
			on_static_mesh_collider_modified(*_modified_static_mesh_collider_channel, _modified_static_mesh_collider_sid, *_data, scene);
			on_static_mesh_collider_destroyed(*_destroyed_static_mesh_collider_channel, _destroyed_static_mesh_collider_sid, *_data);

			// Consume rigid body events
			on_rigid_body_new(scene, *_new_rigid_body_channel, _new_rigid_body_sid, *_data);
			on_rigid_body_modified(*_modified_rigid_body_channel, _modified_rigid_body_sid, *_data);
			on_rigid_body_modified(*_destroyed_rigid_body_channel, _destroyed_rigid_body_sid, *_data);

			// Consume character controller events
			on_character_controller_new(*_new_character_controller_channel, _new_character_controller_sid, *_data, scene);
			on_character_controller_modified(*_modified_character_controller_channel, _modified_character_controller_sid, *_data);
			on_character_controller_jump(*_character_controller_jump_event_channel, _character_controller_jump_sid, *_data);
			on_character_controller_turn(*_character_controller_turn_event_channel, _character_controller_turn_sid, *_data);
			on_character_controller_walk(*_character_controller_walk_event_channel, _character_controller_walk_sid, *_data);
			on_character_controller_destroyed(*_destroyed_character_controller_channel, _destroyed_character_controller_sid, *_data);

			// Consume level portal events
			on_level_portal_new(*_new_level_portal_channel, _new_level_portal_sid, *_data, scene);
			on_level_portal_destroyed(*_destroyed_level_portal_channel, _destroyed_level_portal_sid, *_data);
	    }

	    void BulletPhysicsSystem::phys_tick(Scene& scene, SystemFrame& frame)
        {
			consume_events(scene);

            // Simulate physics
            _data->phys_world.dynamics_world().stepSimulation(frame.time_delta(), 3);

			// Update scene transforms
			const std::size_t num_transforms = _data->frame_transformed_nodes.size();
			auto* const scene_nodes = (Node**)std::malloc(num_transforms * sizeof(Node*));
			scene.get_nodes(_data->frame_transformed_nodes.data(), num_transforms, scene_nodes);
			update_scene_nodes(scene_nodes, _data->frame_transformed_node_transforms.data(), num_transforms);
			std::free(scene_nodes);

			// Handle collision with portal component
			auto* const portal_component_container = scene.get_component_container(CLevelPortal::type_info);
			int numManifolds = _data->phys_world.dynamics_world().getDispatcher()->getNumManifolds();
			for (int i = 0; i < numManifolds; i++)
			{
				auto* const contactManifold = _data->phys_world.dynamics_world().getDispatcher()->getManifoldByIndexInternal(i);
				auto* const obA = contactManifold->getBody0();
				auto* const obB = contactManifold->getBody1();
				if (obA->getUserIndex() == 1 && obB->getUserIndex2() == 1)
				{
					// Get the CLevelPortal for this node
					const auto node_id = static_cast<const PhysicsEntity*>(obB->getUserPointer())->node;
					CLevelPortal* portal_instance;
					portal_component_container->get_instances(&node_id, 1, &portal_instance);
					portal_instance->trigger();
					break;
				}
				if (obB->getUserIndex() == 1 && obA->getUserIndex2() == 1)
				{
					const auto node_id = static_cast<const PhysicsEntity*>(obA->getUserPointer())->node;
					CLevelPortal* portal_instance;
					portal_component_container->get_instances(&node_id, 1, &portal_instance);
					portal_instance->trigger();
					break;
				}
			}

			// Acknowledge transform events (so we don't get stuck in a feedback loop)
			frame.yield();
			_node_world_transform_changed_channel->acknowledge_unconsumed(_node_world_transform_changed_sid);

			// Clear data
            _data->frame_transformed_nodes.clear();
			_data->frame_transformed_node_transforms.clear();
        }

        void BulletPhysicsSystem::debug_draw(Scene& scene, SystemFrame& /*frame*/)
        {
	    consume_events(scene);

            // Draw the world
            DebugDrawer drawer;
            _data->phys_world.dynamics_world().setDebugDrawer(&drawer);
            _data->phys_world.dynamics_world().debugDrawWorld();
            _data->phys_world.dynamics_world().setDebugDrawer(nullptr);

            // Add events to the channel
	    scene.get_debug_draw_line_channel()->append(drawer.lines.data(), (int32)drawer.lines.size());
        }
    }
}
