// BulletPhysicsSystem.h
#pragma once

#include <memory>
#include <Core/Reflection/Reflection.h>
#include <Engine/Components/Gameplay/CCharacterController.h>
#include "build.h"

namespace sge
{
    struct SystemFrame;
    struct UpdatePipeline;
    class CBoxCollider;
    class CCapsuleCollider;
    class CRigidBody;

    namespace bullet_physics
    {
        struct Config;

        struct SGE_BULLET_PHYSICS_API BulletPhysicsSystem
        {
            struct Data;
            SGE_REFLECTED_TYPE;

            ////////////////////////
            ///   Constructors   ///
        public:

            BulletPhysicsSystem(const Config& config);
            ~BulletPhysicsSystem();

            ///////////////////
            ///   Methods   ///
        public:

            void register_pipeline(UpdatePipeline& pipeline);

        private:

            void phys_tick(SystemFrame& frame, float current_time, float dt);

            void debug_draw(SystemFrame& frame, float current_time, float dt);

            void initialize_world(SystemFrame& frame);

            void cb_new_box_collider(
                SystemFrame& frame,
                FNewComponent tag,
                TComponentId<CBoxCollider> component);

            void cb_deleted_box_collider(
                SystemFrame& frame,
                FDestroyedComponent tag,
                TComponentId<CBoxCollider> component);

            void cb_new_capsule_collider(
                SystemFrame& frame,
                FNewComponent tag,
                TComponentId<CCapsuleCollider> component);

            void cb_deleted_capsule_collider(
                SystemFrame& frame,
                FDestroyedComponent tag,
                TComponentId<CCapsuleCollider> component);

            void cb_new_rigid_body(
                SystemFrame& frame,
                FNewComponent tag,
                TComponentId<CRigidBody> component);

            void cb_deleted_rigid_body(
                SystemFrame& frame,
                FDestroyedComponent tag,
                TComponentId<CRigidBody> component);

            void cb_new_character_controller(
                SystemFrame& frame,
                FNewComponent tag,
                TComponentId<CCharacterController> component);

            void cb_deleted_character_controller(
                SystemFrame& frame,
                FDestroyedComponent tag,
                TComponentId<CCharacterController> component);

            void cb_character_controller_walk(
                SystemFrame& frame,
                CCharacterController::FWalkEvent tag,
                TComponentId<CCharacterController> component);

            void cb_character_controller_jump(
                SystemFrame& frame,
                CCharacterController::FJumpEvent tag,
                TComponentId<CCharacterController> component);

            //////////////////
            ///   Fields   ///
        private:

            bool _initialized_world;
            std::unique_ptr<Data> _data;
        };
    }
}
