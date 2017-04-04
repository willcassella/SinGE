// RigidBody.h
#pragma once

#include "BulletPhysicsSystemData.h"

namespace sge
{
	namespace bullet_physics
	{
		void on_new_rigid_body(
			Scene& scene,
			EventChannel& new_rigid_body_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_destroy_rigid_body(
			EventChannel& destroyed_rigid_body_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_rigid_body_modified(
			EventChannel& rigid_body_modified_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);
	}
}
