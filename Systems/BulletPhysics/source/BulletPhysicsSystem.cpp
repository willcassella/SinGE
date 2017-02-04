// BulletPhysicsSystem.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Engine/UpdatePipeline.h>
#include <Engine/Components/CTransform3D.h>
#include <Engine/Components/Physics/CBoxCollider.h>
#include <Engine/Components/Physics/CCapsuleCollider.h>
#include <Engine/Components/Physics/CVelocity.h>
#include <Engine/Components/Physics/CRigidBody.h>
#include <Engine/Components/Gameplay/CCharacterController.h>
#include <Engine/SystemFrame.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"
#include "../private/BulletPhysicsSystemData.h"
#include "../private/PhysicsEntity.h"
#include "../private/DebugDrawer.h"
#include "../private/CharacterController.h"
#include "../private/Util.h"

SGE_REFLECT_TYPE(sge::bullet_physics::BulletPhysicsSystem);

namespace sge
{
    namespace bullet_physics
    {
        BulletPhysicsSystem::BulletPhysicsSystem(const Config& config)
            : _initialized_world(false)
        {
            _data = std::make_unique<Data>();
        }

        BulletPhysicsSystem::~BulletPhysicsSystem()
        {
        }

        void BulletPhysicsSystem::register_pipeline(UpdatePipeline& pipeline)
        {
            const auto phys_async_token = pipeline.new_async_token();

            const auto phys_system = pipeline.register_system_fn(
                "bullet_physics",
                phys_async_token,
                this,
                &BulletPhysicsSystem::phys_tick);

            pipeline.register_system_fn(
                "bullet_physics_debug_draw",
                phys_async_token,
                this,
                &BulletPhysicsSystem::debug_draw);

            pipeline.register_tag_callback<CBoxCollider, FNewComponent>(
                phys_system,
                phys_async_token,
                TCO_NONE,
                this,
                &BulletPhysicsSystem::cb_new_box_collider);

            pipeline.register_tag_callback<CBoxCollider, FDestroyedComponent>(
                phys_system,
                phys_async_token,
                TCO_NONE,
                this,
                &BulletPhysicsSystem::cb_deleted_box_collider);

            pipeline.register_tag_callback<CCapsuleCollider, FNewComponent>(
                phys_system,
                phys_async_token,
                TCO_NONE,
                this,
                &BulletPhysicsSystem::cb_new_capsule_collider);

            pipeline.register_tag_callback<CCapsuleCollider, FDestroyedComponent>(
                phys_system,
                phys_async_token,
                TCO_NONE,
                this,
                &BulletPhysicsSystem::cb_deleted_capsule_collider);

            pipeline.register_tag_callback<CCharacterController, FNewComponent>(
                phys_system,
                phys_async_token,
                TCO_NONE,
                this,
                &BulletPhysicsSystem::cb_new_character_controller);

            pipeline.register_tag_callback<CCharacterController, FDestroyedComponent>(
                phys_system,
                phys_async_token,
                TCO_NONE,
                this,
                &BulletPhysicsSystem::cb_deleted_character_controller);

            pipeline.register_tag_callback<CTransform3D, FModifiedComponent>(
                phys_system,
                phys_async_token,
                TCO_NONE,
                this,
                &BulletPhysicsSystem::cb_modified_transform);

            pipeline.register_tag_callback<CCharacterController, CCharacterController::FJumpEvent>(
                phys_system,
                phys_async_token,
                TCO_NONE,
                this,
                &BulletPhysicsSystem::cb_character_controller_jump);

            pipeline.register_tag_callback<CCharacterController>(
                phys_system,
                phys_async_token,
                TCO_NONE,
                this,
                &BulletPhysicsSystem::cb_character_controller_walk);

            pipeline.register_tag_callback<CCharacterController>(
                phys_system,
                phys_async_token,
                TCO_NONE,
                this,
                &BulletPhysicsSystem::cb_character_controller_turn);
        }

        void BulletPhysicsSystem::phys_tick(SystemFrame& frame, float /*current_time*/, float dt)
        {
            // Initialize the world, if we haven't already
            if (!_initialized_world)
            {
                initialize_world(frame);
                _initialized_world = true;
            }

            // Simulate physics
            _data->phys_world.dynamics_world().stepSimulation(dt, 3);

            // Create an array of the moved entities
            std::vector<EntityId> moved_entities;
            moved_entities.reserve(_data->frame_transformed_entities.size());
            for (auto phys_entity : _data->frame_transformed_entities)
            {
                moved_entities.push_back(phys_entity->entity);
            }

            // Update the transforms
            frame.process_entities_mut(zip_ord_ents(moved_entities.data(), moved_entities.size()),
                [iter = _data->frame_transformed_entities.begin()](
                ProcessingFrame& /*pframe*/,
                sge::CTransform3D& transform) mutable
            {
                from_bullet(transform, (*iter)->transform);
                ++iter;
            });

            // Update velocity
            frame.process_entities_mut(zip_ord_ents(moved_entities.data(), moved_entities.size()),
                [iter = _data->frame_transformed_entities.begin()](
                ProcessingFrame& /*pframe*/,
                sge::CVelocity& velocity) mutable
            {
                velocity.linear_velocity(from_bullet((*iter)->rigid_body->getInterpolationLinearVelocity()));
                velocity.angular_velocity(from_bullet((*iter)->rigid_body->getInterpolationAngularVelocity()));
            });

            _data->frame_transformed_entities.clear();
        }

        void BulletPhysicsSystem::debug_draw(SystemFrame& frame, float current_time, float dt)
        {
            DebugDrawer drawer;

            // Draw the world
            _data->phys_world.dynamics_world().setDebugDrawer(&drawer);
            _data->phys_world.dynamics_world().debugDrawWorld();
            _data->phys_world.dynamics_world().setDebugDrawer(nullptr);

            // TODO
        }

        void BulletPhysicsSystem::initialize_world(SystemFrame& frame)
        {
            // Load all box colliders
            frame.process_entities([&data = *_data](
                ProcessingFrame& pframe,
                const sge::CBoxCollider& box_collider)
            {
                data.add_box_collider(pframe.entity(), box_collider);
            });

            // Load all capsule collider
            frame.process_entities([&data = *_data](
                ProcessingFrame& pframe,
                const sge::CCapsuleCollider& collider)
            {
                data.add_capsule_collider(pframe.entity(), collider);
            });

            // Load all rigid bodies
            frame.process_entities([&data = *_data](
                ProcessingFrame& pframe,
                const sge::CRigidBody& rigid_body,
                const sge::CTransform3D& transform)
            {
                data.add_rigid_body(pframe.entity(), transform, rigid_body);
            });

            // Load all character controllers
            frame.process_entities([&data = *_data](
                ProcessingFrame& pframe,
                const sge::CCharacterController& character_controller,
                const sge::CTransform3D& transform)
            {
                data.add_character_controller(pframe.entity(), transform, character_controller);
            });
        }

        void BulletPhysicsSystem::cb_new_transform(
            SystemFrame& frame,
            const EntityId* entities,
            std::size_t num)
        {

        }

        void BulletPhysicsSystem::cb_modified_transform(
            SystemFrame& frame,
            const EntityId* entities,
            std::size_t num_tags)
        {
            frame.process_entities(zip_ord_ents(entities, num_tags), [&data = *_data](
                ProcessingFrame& pframe,
                const CTransform3D& transform)
            {
                auto* phys_entity = data.get_physics_entity(pframe.entity());
                if (!phys_entity)
                {
                    return ProcessControl::CONTINUE;
                }

                btTransform bullet_transform;
                to_bullet(bullet_transform, transform);
                phys_entity->extern_set_transform(bullet_transform);

                return ProcessControl::CONTINUE;
            });
        }

        void BulletPhysicsSystem::cb_new_box_collider(
            SystemFrame& frame,
            const EntityId* entities,
            std::size_t num)
        {
            frame.process_entities(zip_ord_ents(entities, num),
                [&data = *_data](
                ProcessingFrame& pframe,
                const CBoxCollider& box_collider)
            {
                data.add_box_collider(pframe.entity(), box_collider);
            });
        }

        void BulletPhysicsSystem::cb_deleted_box_collider(
            SystemFrame& /*frame*/,
            const EntityId* entities,
            std::size_t num)
        {
            for (std::size_t i = 0; i < num; ++i)
            {
                _data->remove_box_collider(entities[i]);
            }
        }

        void BulletPhysicsSystem::cb_new_capsule_collider(
            SystemFrame& frame,
            const EntityId* entities,
            std::size_t num)
        {
            frame.process_entities(zip_ord_ents(entities, num),
                [&data = *_data](
                ProcessingFrame& pframe,
                const CCapsuleCollider& collider)
            {
                data.add_capsule_collider(pframe.entity(), collider);
            });
        }

        void BulletPhysicsSystem::cb_deleted_capsule_collider(
            SystemFrame& /*frame*/,
            const EntityId* entities,
            std::size_t num)
        {
            for (std::size_t i = 0; i < num; ++i)
            {
                _data->remove_capsule_collider(entities[i]);
            }
        }

        void BulletPhysicsSystem::cb_new_rigid_body(
            SystemFrame& frame,
            const EntityId* entities,
            std::size_t num)
        {
            frame.process_entities(zip_ord_ents(entities, num),
                [&data = *_data](
                ProcessingFrame& pframe,
                const sge::CTransform3D& transform,
                const sge::CRigidBody& rigid_body)
            {
                data.add_rigid_body(pframe.entity(), transform, rigid_body);
            });
        }

        void BulletPhysicsSystem::cb_deleted_rigid_body(
            SystemFrame& /*frame*/,
            const EntityId* entities,
            std::size_t num)
        {
            for (std::size_t i = 0; i < num; ++i)
            {
                _data->remove_rigid_body(entities[i]);
            }
        }

        void BulletPhysicsSystem::cb_new_character_controller(
            SystemFrame& frame,
            const EntityId* entities,
            std::size_t num)
        {
            frame.process_entities(zip_ord_ents(entities, num),
                [&data = *_data](
                ProcessingFrame& pframe,
                const CCharacterController& character_controller,
                const CTransform3D& transform)
            {
                data.add_character_controller(pframe.entity(), transform, character_controller);
            });
        }

        void BulletPhysicsSystem::cb_deleted_character_controller(
            SystemFrame& /*frame*/,
            const EntityId* entities,
            std::size_t num)
        {
            for (std::size_t i = 0; i < num; ++i)
            {
                _data->remove_character_contoller(entities[i]);
            }
        }

        void BulletPhysicsSystem::cb_character_controller_jump(
            SystemFrame& /*frame*/,
            const EntityId* entities,
            std::size_t num)
        {
            for (std::size_t i = 0; i < num; ++i)
            {
                auto* phys_entity = _data->get_physics_entity(entities[i]);

                if (!phys_entity || !phys_entity->character_controller)
                {
                    continue;
                }

                if (phys_entity->character_controller->canJump())
                {
                    phys_entity->character_controller->jump(btVector3{ 0, 0, 0 });
                }
            }
        }

        void BulletPhysicsSystem::cb_character_controller_walk(
            SystemFrame& /*frame*/,
            const EntityId* entities,
            const TagCount_t* tag_counts,
            std::size_t num_ents,
            const CCharacterController::FWalkEvent* tags)
        {
            for (std::size_t i = 0; i < num_ents; ++i)
            {
                auto* phys_entity = _data->get_physics_entity(entities[i]);

                for (TagCount_t count_i = 0; count_i < tag_counts[i]; ++count_i, ++tags)
                {
                    phys_entity->character_controller->walk(tags->direction);
                }
            }
        }

        void BulletPhysicsSystem::cb_character_controller_turn(
            SystemFrame& /*frame*/,
            const EntityId* entities,
            const TagCount_t* tag_counts,
            std::size_t num_ents,
            const CCharacterController::FTurnEvent* tags)
        {
            for (std::size_t i = 0; i < num_ents; ++i)
            {
                auto* phys_entity = _data->get_physics_entity(entities[i]);
                if (!phys_entity || !phys_entity->character_controller)
                {
                    continue;
                }

                phys_entity->character_controller->turn(tags[i].amount);
            }
        }
    }
}
