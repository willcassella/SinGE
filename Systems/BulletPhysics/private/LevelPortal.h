// LevelPortal.h
#pragma once

#include <btBulletCollisionCommon.h>
#include <Engine/Components/Gameplay/CLevelPortal.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"

namespace sge
{
	namespace bullet_physics
	{
		void on_level_portal_new(
			EventChannel& new_level_portal_channel,
			EventChannel::SubscriberId new_level_portal_sid,
			BulletPhysicsSystem::Data& phys_data,
			Scene& scene);

		void on_level_portal_destroyed(
			EventChannel& destroyed_level_portal_channel,
			EventChannel::SubscriberId destroyed_level_portal_sid,
			BulletPhysicsSystem::Data& phys_data);
	}
}
