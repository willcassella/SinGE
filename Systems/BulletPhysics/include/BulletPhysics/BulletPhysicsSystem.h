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

            void cb_modified_transform(
                SystemFrame& frame,
                const FModifiedComponent* tags,
                const EntityId* entities,
                std::size_t num_tags);

            void cb_new_box_collider(
                SystemFrame& frame,
                const FNewComponent* tags,
                const EntityId* entities,
                std::size_t num);

            void cb_deleted_box_collider(
                SystemFrame& frame,
                const FDestroyedComponent* tags,
                const EntityId* entities,
                std::size_t num);

            void cb_new_capsule_collider(
                SystemFrame& frame,
                const FNewComponent* tags,
                const EntityId* entities,
                std::size_t num);

            void cb_deleted_capsule_collider(
                SystemFrame& frame,
                const FDestroyedComponent* tags,
                const EntityId* entities,
                std::size_t num);

            void cb_new_rigid_body(
                SystemFrame& frame,
                const FNewComponent* tags,
                const EntityId* entities,
                std::size_t num);

            void cb_deleted_rigid_body(
                SystemFrame& frame,
                const FDestroyedComponent* tags,
                const EntityId* entities,
                std::size_t num);

            void cb_new_character_controller(
                SystemFrame& frame,
                const FNewComponent* tags,
                const EntityId* entities,
                std::size_t num);

            void cb_deleted_character_controller(
                SystemFrame& frame,
                const FDestroyedComponent* tags,
                const EntityId* entities,
                std::size_t num);

            void cb_character_controller_jump(
                SystemFrame& frame,
                const CCharacterController::FJumpEvent* tags,
                const EntityId* entities,
                std::size_t num);

            void cb_character_controller_walk(
                SystemFrame& frame,
                const CCharacterController::FWalkEvent* tags,
                const EntityId* entities,
                std::size_t num);

            void cb_character_controller_turn(
                SystemFrame& frame,
                const CCharacterController::FTurnEvent* tags,
                const EntityId* entities,
                std::size_t num);

            //////////////////
            ///   Fields   ///
        private:

            bool _initialized_world;
            std::unique_ptr<Data> _data;
        };
    }
}
