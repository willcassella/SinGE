#pragma once

#include <memory>

#include "lib/base/reflection/reflection.h"
#include "lib/bullet_physics/build.h"
#include "lib/engine/components/gameplay/character_controller.h"

namespace sge
{
    struct SystemFrame;
    struct UpdatePipeline;

    namespace bullet_physics
    {
        struct Config;

        struct SGE_BULLET_PHYSICS_API BulletPhysicsSystem
        {
            struct Data;
            SGE_REFLECTED_TYPE;

            BulletPhysicsSystem(const Config& config);
            ~BulletPhysicsSystem();

            void register_pipeline(UpdatePipeline& pipeline);

            void initialize_subscriptions(Scene& scene);

            void reset();

        private:
            void consume_events(Scene& scene);

            void phys_tick(Scene& scene, SystemFrame& frame);

            void debug_draw(Scene& scene, SystemFrame& frame);

            std::unique_ptr<Data> _data;
            EventChannel* _node_world_transform_changed_channel;
            EventChannel* _new_rigid_body_channel;
            EventChannel* _modified_rigid_body_channel;
            EventChannel* _destroyed_rigid_body_channel;
            EventChannel* _new_sphere_collider_channel;
            EventChannel* _modified_sphere_collider_channel;
            EventChannel* _destroyed_sphere_collider_channel;
            EventChannel* _new_box_collider_channel;
            EventChannel* _modified_box_collider_channel;
            EventChannel* _destroyed_box_collider_channel;
            EventChannel* _new_capsule_collider_channel;
            EventChannel* _modified_capsule_collider_channel;
            EventChannel* _destroyed_capsule_collider_channel;
            EventChannel* _new_static_mesh_collider_channel;
            EventChannel* _modified_static_mesh_collider_channel;
            EventChannel* _destroyed_static_mesh_collider_channel;
            EventChannel* _new_character_controller_channel;
            EventChannel* _modified_character_controller_channel;
            EventChannel* _character_controller_jump_event_channel;
            EventChannel* _character_controller_turn_event_channel;
            EventChannel* _character_controller_walk_event_channel;
            EventChannel* _destroyed_character_controller_channel;
            EventChannel* _new_level_portal_channel;
            EventChannel* _destroyed_level_portal_channel;
            EventChannel* _new_spotlight_channel;
            EventChannel* _destroyed_spotlight_channel;
            EventChannel::SubscriberId _node_world_transform_changed_sid;
            EventChannel::SubscriberId _new_rigid_body_sid;
            EventChannel::SubscriberId _modified_rigid_body_sid;
            EventChannel::SubscriberId _destroyed_rigid_body_sid;
            EventChannel::SubscriberId _new_sphere_collider_sid;
            EventChannel::SubscriberId _modified_sphere_collider_sid;
            EventChannel::SubscriberId _destroyed_sphere_collider_sid;
            EventChannel::SubscriberId _new_box_collider_sid;
            EventChannel::SubscriberId _modified_box_collider_sid;
            EventChannel::SubscriberId _destroyed_box_collider_sid;
            EventChannel::SubscriberId _new_capsule_collider_sid;
            EventChannel::SubscriberId _modified_capsule_collider_sid;
            EventChannel::SubscriberId _destroyed_capsule_collider_sid;
            EventChannel::SubscriberId _new_static_mesh_collider_sid;
            EventChannel::SubscriberId _modified_static_mesh_collider_sid;
            EventChannel::SubscriberId _destroyed_static_mesh_collider_sid;
            EventChannel::SubscriberId _new_character_controller_sid;
            EventChannel::SubscriberId _modified_character_controller_sid;
            EventChannel::SubscriberId _character_controller_jump_sid;
            EventChannel::SubscriberId _character_controller_turn_sid;
            EventChannel::SubscriberId _character_controller_walk_sid;
            EventChannel::SubscriberId _destroyed_character_controller_sid;
            EventChannel::SubscriberId _new_level_portal_sid;
            EventChannel::SubscriberId _destroyed_level_portal_sid;
            EventChannel::SubscriberId _new_spotlight_sid;
            EventChannel::SubscriberId _destroyed_spotlight_sid;
        };
    }
}
