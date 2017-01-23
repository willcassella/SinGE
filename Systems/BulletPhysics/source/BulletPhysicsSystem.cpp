// BulletPhysicsSystem.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Engine/UpdatePipeline.h>
#include <Engine/Components/CTransform3D.h>
#include <Engine/Components/Physics/CBoxCollider.h>
#include <Engine/Components/Physics/CVelocity.h>
#include <Engine/Components/Physics/CRigidBody.h>
#include <Engine/SystemFrame.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"
#include "../private/BulletPhysicsSystemData.h"
#include "../private/PhysicsEntity.h"
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
            pipeline.register_tag_callback(this, &BulletPhysicsSystem::cb_new_box_collider);
            pipeline.register_tag_callback(this, &BulletPhysicsSystem::cb_deleted_box_collider);
        }

        void BulletPhysicsSystem::phys_tick(SystemFrame& frame, float current_time, float dt)
        {
            // Initialize the world, if we haven't already
            if (!_initialized_world)
            {
                initialize_world(frame);
                _initialized_world = true;
            }

            // Add any rigid bodies to the world

            // Simulate physics
            _data->phys_world.dynamics_world().stepSimulation(dt, 1);

            // For each entity that was moved during the simulation
            for (auto phys_entity : _data->frame_transformed_entities)
            {
                // Update the transform
                frame.process_single_mut(phys_entity->entity, [phys_entity](
                    ProcessingFrame& /*pframe*/,
                    EntityId /*entity*/,
                    sge::CTransform3D& transform)
                {
                    transform.set_local_position(from_bullet(phys_entity->position));
                    transform.set_local_rotation(from_bullet(phys_entity->rotation));

                });

                // Update velocity
                frame.process_single_mut(phys_entity->entity, [phys_entity](
                    ProcessingFrame& /*pframe*/,
                    EntityId /*entity*/,
                    sge::CVelocity& velocity)
                {
                    velocity.linear_velocity(from_bullet(phys_entity->rigid_body->getInterpolationLinearVelocity()));
                    velocity.angular_velocity(from_bullet(phys_entity->rigid_body->getInterpolationAngularVelocity()));
                });
            }

            _data->frame_transformed_entities.clear();
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

            // Load all rigid bodies
            frame.process_entities([&data = *_data](
                ProcessingFrame& /*pframe*/,
                EntityId entity,
                const sge::CRigidBody& rigid_body,
                const sge::CTransform3D& transform)
            {
                data.add_rigid_body(entity, transform, rigid_body);
            });
        }

        void BulletPhysicsSystem::cb_new_box_collider(
            SystemFrame& frame,
            FNewComponent /*tag*/,
            TComponentId<CBoxCollider> component)
        {
            frame.process_single(component.entity(), [&data = *_data](
                ProcessingFrame& /*pframe*/,
                EntityId entity,
                const CBoxCollider& box_collider)
            {
                data.add_box_collider(entity, box_collider);
            });
        }

        void BulletPhysicsSystem::cb_deleted_box_collider(
            SystemFrame& /*frame*/,
            FDestroyedComponent /*tag*/,
            TComponentId<CBoxCollider> component)
        {
            _data->remove_box_collider(component.entity());
        }

        void BulletPhysicsSystem::cb_new_rigid_body(
            SystemFrame& frame,
            FNewComponent /*tag*/,
            TComponentId<CRigidBody> component)
        {
            bool created = false;
            frame.process_single(component.entity(), [&created, &data = *_data](
                ProcessingFrame& /*pframe*/,
                EntityId entity,
                const sge::CTransform3D& transform,
                const sge::CRigidBody& rigid_body)
            {
                created = true;
                data.add_rigid_body(entity, transform, rigid_body);
            });
        }

        void BulletPhysicsSystem::cb_deleted_rigid_body(
            SystemFrame& /*frame*/,
            FDestroyedComponent /*tag*/,
            TComponentId<CRigidBody> /*component*/)
        {
            //_data->remove_rigid_body(entity);
        }
    }
}
