// Colliders.h
#pragma once

#include "BulletPhysicsSystemData.h"

namespace sge
{
	namespace bullet_physics
	{
		void on_box_collider_new(
			EventChannel& new_box_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_box_collider_destroyed(
			EventChannel& destroyed_box_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_box_collider_modified(
			EventChannel& modified_box_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_capsule_collider_new(
			EventChannel& new_capsule_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_capsule_collider_destroyed(
			EventChannel& destroyed_capsule_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_capsule_collider_modified(
			EventChannel& modifed_capsule_collider_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);
	}
}
