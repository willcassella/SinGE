#include "lib/base/math/mat4.h"
#include "lib/bullet_physics/bullet_physics_system_data.h"
#include "lib/bullet_physics/character_controller.h"
#include "lib/bullet_physics/physics_entity.h"
#include "lib/bullet_physics/util.h"
#include "lib/engine/components/gameplay/character_controller.h"
#include "lib/engine/scene.h"

namespace sge
{
    namespace bullet_physics
    {
        CharacterController::CharacterController(
            PhysicsEntity& phys_entity,
            const CCharacterController& component)
            : btKinematicCharacterController(
                nullptr,
                phys_entity.capsule_collider.get(),
                component.step_height(),
                btVector3{ 0, 1, 0 }),
            _phys_entity(&phys_entity)
        {
            // Initialize character controller properties
            setFallSpeed(component.fall_speed());
            setMaxSlope(component.max_slope().radians());
            setJumpSpeed(component.jump_speed());

            // Set ghost object
            ghost_object.setCollisionShape(phys_entity.capsule_collider.get());
            ghost_object.setCollisionFlags(ghost_object.getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);
            ghost_object.setWorldTransform(phys_entity.transform);
            ghost_object.setUserIndex(phys_entity.get_user_index_1());
            ghost_object.setUserIndex2(phys_entity.get_user_index_2());
            ghost_object.setUserPointer(&phys_entity);
            m_ghostObject = &ghost_object;
        }

        void CharacterController::updateAction(btCollisionWorld* world, btScalar deltaTimeStep)
        {
            // Set walk direction
            setWalkDirection(to_bullet(_walk_dir.normalized()) / 10);

            // Set angular velocity
            setAngularVelocity(btVector3{ 0, _turn_amount.degrees(), 0 });

            // Call base implementation
            btKinematicCharacterController::updateAction(world, deltaTimeStep);

            // Update entity transform
            const auto& transform = getGhostObject()->getWorldTransform();
            _phys_entity->transform = transform;
            _phys_entity->add_to_modified();

            // Reset walk direction
            setWalkDirection(btVector3{ 0, 0, 0 });
            _walk_dir = Vec3::zero();

            // Reset turn amount
            setAngularVelocity(btVector3{ 0, 0, 0 });
            _turn_amount = 0;
        }

        void CharacterController::walk(Vec2 dir)
        {
            const Vec3 added_walk_dir{ dir.x(), 0, -dir.y() };
            _walk_dir += Mat4::rotate(from_bullet(_phys_entity->transform.getRotation())) * added_walk_dir;
        }

        void CharacterController::turn(Angle amount)
        {
            _turn_amount = _turn_amount + amount;
        }

        bool CharacterController::needsCollision(
            const btCollisionObject* body0,
            const btCollisionObject* body1)
        {
            const auto last_frame_id = _phys_entity->phys_data->last_frame_id;

            // If we collided with a lighmask receiver
            if (body0->getUserIndex() & LIGHTMASK_RECEIVER_BIT && last_lightmask_collision_frame < last_frame_id - 1)
            {
                return false;
            }
            if (body1->getUserIndex() & LIGHTMASK_RECEIVER_BIT && last_lightmask_collision_frame < last_frame_id - 1)
            {
                return false;
            }

            return true;
        }

        void on_character_controller_new(
            EventChannel& new_character_controller_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data,
            Scene& scene)
        {
            // Get events
            ENewComponent events[8];
            int32 num_events;
            while (new_character_controller_channel.consume(subscriber_id, events, &num_events))
            {
                NodeId node_ids[8];
                const Node* nodes[8];
                const CCharacterController* components[8];

                // Gather node ids and instances
                for (int32 i = 0; i < num_events; ++i)
                {
                    node_ids[i] = events[i].node;
                    components[i] = (const CCharacterController*)events[i].instance;
                }

                // Get nodes
                scene.get_nodes(node_ids, num_events, nodes);

                // Iterate over events
                for (int32 i = 0; i < num_events; ++i)
                {
                    auto& physics_entity = phys_data.get_or_create_physics_entity(node_ids[i], *nodes[i]);
                    assert(physics_entity.character_controller == nullptr);

                    // Create the character controller
                    physics_entity.character_controller = std::make_unique<CharacterController>(physics_entity, *components[i]);

                    // Set the character collider bit
                    physics_entity.set_user_index_1(physics_entity.get_user_index_1() | CHARACTER_BIT);

                    // Add the ghost object to the world
                    phys_data.phys_world.dynamics_world().addCollisionObject(&physics_entity.character_controller->ghost_object,
                        btBroadphaseProxy::CharacterFilter,
                        btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::SensorTrigger);

                    // Add the character controller to the world
                    phys_data.phys_world.dynamics_world().addAction(physics_entity.character_controller.get());

                    // Figure out if we need to delete the collision object
                    phys_data.post_add_physics_entity_element(physics_entity);
                }
            }
        }

        void on_character_controller_destroyed(
            EventChannel& destroyed_character_controller_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data)
        {
            // Get events
            EDestroyedComponent events[8];
            int32 num_events;
            while (destroyed_character_controller_channel.consume(subscriber_id, events, &num_events))
            {
                for (int32 i = 0; i < num_events; ++i)
                {
                    auto* const phys_entity = phys_data.get_physics_entity(events[i].node);
                    assert(phys_entity != nullptr && phys_entity->character_controller != nullptr);

                    // Remove it from the world
                    phys_data.phys_world.dynamics_world().removeAction(phys_entity->character_controller.get());
                    phys_data.phys_world.dynamics_world().removeCollisionObject(&phys_entity->character_controller->ghost_object);
                    phys_entity->character_controller = nullptr;

                    phys_entity->set_user_index_1(phys_entity->get_user_index_1() & ~CHARACTER_BIT);

                    // Evaluate if we still need this physics object
                    phys_data.post_remove_physics_entity_element(*phys_entity);
                }
            }
        }

        void on_character_controller_modified(
            EventChannel& modified_character_controller_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data)
        {
            // Get events
            EModifiedComponent events[8];
            int32 num_events;
            while (modified_character_controller_channel.consume(subscriber_id, events, &num_events))
            {
                for (int32 i = 0; i < num_events; ++i)
                {
                    const auto* const component = (const CCharacterController*)events[i].instance;

                    auto* const phys_entity = phys_data.get_physics_entity(events[i].node);
                    assert(phys_entity != nullptr && phys_entity->character_controller != nullptr);
                    auto* const character_controller = phys_entity->character_controller.get();

                    // Update properties
                    character_controller->setStepHeight(component->step_height());
                    character_controller->setMaxSlope(component->max_slope());
                    character_controller->setJumpSpeed(component->jump_speed());
                    character_controller->setFallSpeed(component->fall_speed());
                }
            }
        }

        void on_character_controller_jump(
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
                    if (phys_ent->character_controller->onGround())
                    {
                        phys_ent->character_controller->jump(btVector3{ 0.0, 0.0, 0.0 });
                    }
                }
            }
        }

        void on_character_controller_turn(
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

        void on_character_controller_walk(
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
    }
}
