// CharacterController.h
#pragma once

#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <Core/Math/Vec3.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"

namespace sge
{
    struct CCharacterController;

    namespace bullet_physics
    {
        class PhysicsEntity;

        class CharacterController final : public btKinematicCharacterController
        {
            ////////////////////////
            ///   Constructors   ///
        public:

            CharacterController(
                PhysicsEntity& phys_enitty,
                const CCharacterController& component);

            ///////////////////
            ///   Methods   ///
        public:

            void updateAction(btCollisionWorld* world, btScalar deltaTimeStep) override;

            void walk(Vec2 dir);

            void turn(Angle amount);

			bool needsCollision(
				const btCollisionObject* body0,
				const btCollisionObject* body1) override;

            //////////////////
            ///   Fields   ///
        public:

            btPairCachingGhostObject ghost_object;

        private:

			uint64 _last_lightmask_collision_frame = 0;
            PhysicsEntity* _phys_entity;
            Angle _turn_amount;
            Vec3 _walk_dir;
        };

		void on_character_controller_new(
			EventChannel& new_character_controller_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data,
			Scene& scene);

		void on_character_controller_destroyed(
			EventChannel& destroyed_character_controller_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_character_controller_modified(
			EventChannel& modified_character_controller_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_character_controller_jump(
			EventChannel& jump_event_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_character_controller_turn(
			EventChannel& turn_event_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);

		void on_character_controller_walk(
			EventChannel& walk_event_channel,
			EventChannel::SubscriberId subscriber_id,
			BulletPhysicsSystem::Data& phys_data);
    }
}
