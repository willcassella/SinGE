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
            pipeline.register_system_fn("bullet_physics", this, &BulletPhysicsSystem::phys_tick);
            pipeline.register_system_fn("bullet_physics_debug_draw", this, &BulletPhysicsSystem::debug_draw);
            //pipeline.register_tag_callback(this, &BulletPhysicsSystem::cb_new_box_collider);
            //pipeline.register_tag_callback(this, &BulletPhysicsSystem::cb_deleted_box_collider);
            //pipeline.register_tag_callback(this, &BulletPhysicsSystem::cb_new_capsule_collider);
            //pipeline.register_tag_callback(this, &BulletPhysicsSystem::cb_deleted_capsule_collider);
            //pipeline.register_tag_callback(this, &BulletPhysicsSystem::cb_new_character_controller);
            //pipeline.register_tag_callback(this, &BulletPhysicsSystem::cb_deleted_character_controller);
            //pipeline.register_tag_callback(this, &BulletPhysicsSystem::cb_character_controller_walk);
            //pipeline.register_tag_callback(this, &BulletPhysicsSystem::cb_character_controller_jump);
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
            _data->phys_world.dynamics_world().stepSimulation(dt, 1);

            // Create an array of the moved entities
            std::vector<EntityId> moved_entities;
            moved_entities.reserve(_data->frame_transformed_entities.size());
            for (auto phys_entity : _data->frame_transformed_entities)
            {
                moved_entities.push_back(phys_entity->entity);
            }

            // Update the transforms
            frame.process_entities_mut(moved_entities.data(), moved_entities.size(),
                [iter = _data->frame_transformed_entities.begin()](
                ProcessingFrame& /*pframe*/,
                EntityId /*entity*/,
                sge::CTransform3D& transform) mutable
            {
                from_bullet(transform, (*iter)->transform);
                ++iter;
            });

            // Update velocity
            frame.process_entities_mut(moved_entities.data(), moved_entities.size(),
                [iter = _data->frame_transformed_entities.begin()](
                ProcessingFrame& /*pframe*/,
                EntityId /*entity*/,
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
                ProcessingFrame&,
                EntityId entity,
                const sge::CBoxCollider& box_collider)
            {
                data.add_box_collider(entity, box_collider);
            });

            // Load all capsule collider
            frame.process_entities([&data = *_data](
                ProcessingFrame& /*pframe*/,
                EntityId entity,
                const sge::CCapsuleCollider& collider)
            {
                data.add_capsule_collider(entity, collider);
            });

            // Load all rigid bodies
            frame.process_entities([&data = *_data](
                ProcessingFrame& /*pframe*/,
                EntityId entity,
                const sge::CRigidBody& rigid_body,
                const sge::CTransform3D& transform)
            {
                data.add_rigid_body(entity, transform, rigid_body);
            });

            // Load all character controllers
            frame.process_entities([&data = *_data](
                ProcessingFrame& /*pframe*/,
                EntityId entity,
                const sge::CCharacterController& character_controller,
                const sge::CTransform3D& transform)
            {
                data.add_character_controller(entity, transform, character_controller);
            });
        }

        void BulletPhysicsSystem::cb_new_box_collider(
            SystemFrame& frame,
            FNewComponent /*tag*/,
            TComponentId<CBoxCollider> component)
        {
            //frame.process_single(component.entity(), [&data = *_data](
            //    ProcessingFrame& /*pframe*/,
            //    EntityId entity,
            //    const CBoxCollider& box_collider)
            //{
            //    data.add_box_collider(entity, box_collider);
            //});
        }

        void BulletPhysicsSystem::cb_deleted_box_collider(
            SystemFrame& /*frame*/,
            FDestroyedComponent /*tag*/,
            TComponentId<CBoxCollider> component)
        {
            _data->remove_box_collider(component.entity());
        }

        void BulletPhysicsSystem::cb_new_capsule_collider(
            SystemFrame& frame,
            FNewComponent /*tag*/,
            TComponentId<CCapsuleCollider> component)
        {
            //frame.process_single(component.entity(), [&data = *_data](
            //    ProcessingFrame& /*pframe*/,
            //    EntityId entity,
            //    const CCapsuleCollider& collider)
            //{
            //    data.add_capsule_collider(entity, collider);
            //});
        }

        void BulletPhysicsSystem::cb_deleted_capsule_collider(
            SystemFrame& /*frame*/,
            FDestroyedComponent /*tag*/,
            TComponentId<CCapsuleCollider> component)
        {
            _data->remove_capsule_collider(component.entity());
        }

        void BulletPhysicsSystem::cb_new_rigid_body(
            SystemFrame& frame,
            FNewComponent /*tag*/,
            TComponentId<CRigidBody> component)
        {
            //bool created = false;
            //frame.process_single(component.entity(), [&created, &data = *_data](
            //    ProcessingFrame& /*pframe*/,
            //    EntityId entity,
            //    const sge::CTransform3D& transform,
            //    const sge::CRigidBody& rigid_body)
            //{
            //    created = true;
            //    data.add_rigid_body(entity, transform, rigid_body);
            //});
        }

        void BulletPhysicsSystem::cb_deleted_rigid_body(
            SystemFrame& /*frame*/,
            FDestroyedComponent /*tag*/,
            TComponentId<CRigidBody> /*component*/)
        {
            //_data->remove_rigid_body(entity);
        }

        void BulletPhysicsSystem::cb_new_character_controller(
            SystemFrame& frame,
            FNewComponent /*tag*/,
            TComponentId<CCharacterController> component)
        {
            //frame.process_single(component.entity(), [&data = *_data](
            //    ProcessingFrame& /*pframe*/,
            //    EntityId entity,
            //    const CCharacterController& character_controller,
            //    const CTransform3D& transform)
            //{
            //    data.add_character_controller(entity, transform, character_controller);
            //});
        }

        void BulletPhysicsSystem::cb_deleted_character_controller(
            SystemFrame& /*frame*/,
            FDestroyedComponent /*tag*/,
            TComponentId<CCharacterController> component)
        {
            _data->remove_character_contoller(component.entity());
        }

        void BulletPhysicsSystem::cb_character_controller_walk(
            SystemFrame& /*frame*/,
            CCharacterController::FWalkEvent tag,
            TComponentId<CCharacterController> component)
        {
            _data->get_physics_entity(component.entity()).character_controller->walk(tag.direction);
        }

        void BulletPhysicsSystem::cb_character_controller_jump(
            SystemFrame& /*frame*/,
            CCharacterController::FJumpEvent /*tag*/,
            TComponentId<CCharacterController> component)
        {
            _data->get_physics_entity(component.entity()).character_controller->jump(btVector3{ 0, 0, 0 });
        }
    }
}
