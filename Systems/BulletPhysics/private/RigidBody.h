// RigidBody.h
#pragma once

#include "BulletPhysicsSystemData.h"

namespace sge
{
	namespace bullet_physics
	{
		void on_rigid_body_new(
			Scene& scene,
			EventChannel& new_rigid_body_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_rigid_body_destroyed(
			EventChannel& destroyed_rigid_body_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_rigid_body_modified(
			EventChannel& modified_rigid_body_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);
	}
}
