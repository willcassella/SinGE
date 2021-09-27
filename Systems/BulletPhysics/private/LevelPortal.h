// LevelPortal.h
#pragma once

#include <Engine/Components/Gameplay/CLevelPortal.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"

namespace sge
{
    namespace bullet_physics
    {
        void on_level_portal_new(
            EventChannel& new_level_portal_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data,
            Scene& scene);

        void on_level_portal_destroyed(
            EventChannel& destroyed_level_portal_channel,
            EventChannel::SubscriberId subsriber_id,
            BulletPhysicsSystem::Data& phys_data);
    }
}
