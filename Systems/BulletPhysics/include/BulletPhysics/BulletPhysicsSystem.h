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

            void cb_new_transform(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num);

            void cb_modified_transform(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num_tags);

            void cb_new_box_collider(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num);

            void cb_deleted_box_collider(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num);

            void cb_new_capsule_collider(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num);

            void cb_deleted_capsule_collider(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num);

            void cb_new_rigid_body(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num_ents);

            void cb_deleted_rigid_body(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num_ents);

            void cb_new_character_controller(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num_ents);

            void cb_deleted_character_controller(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num_ents);

            void cb_character_controller_jump(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num_ents);

            void cb_character_controller_walk(
                SystemFrame& frame,
                const EntityId* entities,
                const TagCount_t* tag_counts,
                std::size_t num_ents,
                const CCharacterController::FWalkEvent* tags);

            void cb_character_controller_turn(
                SystemFrame& frame,
                const EntityId* entities,
                const TagCount_t* tag_counts,
                std::size_t num_ents,
                const CCharacterController::FTurnEvent* tags);

            //////////////////
            ///   Fields   ///
        private:

            bool _initialized_world;
            std::unique_ptr<Data> _data;
        };
    }
}
