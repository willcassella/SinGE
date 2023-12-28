#pragma once

#include "lib/bullet_physics/bullet_physics_system.h"
#include "lib/engine/components/gameplay/level_portal.h"

namespace sge {
namespace bullet_physics {
void on_level_portal_new(
    EventChannel& new_level_portal_channel,
    EventChannel::SubscriberId subscriber_id,
    BulletPhysicsSystem::Data& phys_data,
    Scene& scene
);

void on_level_portal_destroyed(
    EventChannel& destroyed_level_portal_channel,
    EventChannel::SubscriberId subsriber_id,
    BulletPhysicsSystem::Data& phys_data
);
}  // namespace bullet_physics
}  // namespace sge
