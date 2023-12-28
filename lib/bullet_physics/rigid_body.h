#pragma once

#include "lib/bullet_physics/bullet_physics_system_data.h"

namespace sge {
namespace bullet_physics {
void on_rigid_body_new(
    Scene& scene,
    EventChannel& new_rigid_body_channel,
    EventChannel::SubscriberId subscriber_id,
    BulletPhysicsSystem::Data& phys_data
);

void on_rigid_body_destroyed(
    EventChannel& destroyed_rigid_body_channel,
    EventChannel::SubscriberId subscriber_id,
    BulletPhysicsSystem::Data& phys_data
);

void on_rigid_body_modified(
    EventChannel& modified_rigid_body_channel,
    EventChannel::SubscriberId subscriber_id,
    BulletPhysicsSystem::Data& phys_data
);
}  // namespace bullet_physics
}  // namespace sge
