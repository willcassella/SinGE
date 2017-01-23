// BulletPhysicsSystemData.cpp

#include <Engine/Components/Physics/CBoxCollider.h>
#include <Engine/Components/Physics/CRigidBody.h>
#include <Engine/Components/CTransform3D.h>
#include "../private/BulletPhysicsSystemData.h"
#include "../private/PhysicsEntity.h"
#include "../private/Util.h"

namespace sge
{
    namespace bullet_physics
    {
        void BulletPhysicsSystem::Data::add_box_collider(EntityId entity, const CBoxCollider& component)
        {
            auto& physics_entity = get_or_create_physics_entity(entity);
            assert(physics_entity.box_collider == nullptr);
            physics_entity.box_collider = std::make_unique<btBoxShape>(to_bullet(component.shape() / 2));
            physics_entity.collider.addChildShape(btTransform::getIdentity(), physics_entity.box_collider.get());
        }

        void BulletPhysicsSystem::Data::remove_box_collider(EntityId entity)
        {
            auto& phys_entity = get_physics_entity(entity);
            assert(phys_entity.box_collider != nullptr);

            // Remove the box collider from the compound, and the physisc entity
            phys_entity.collider.removeChildShape(phys_entity.box_collider.get());
            phys_entity.box_collider = nullptr;

            post_modify_physics_entity(phys_entity);
        }

        void BulletPhysicsSystem::Data::add_rigid_body(EntityId entity, const CTransform3D& transform, const CRigidBody& component)
        {
            auto& physics_entity = get_or_create_physics_entity(entity);
            assert(physics_entity.rigid_body == nullptr);

            // Move the physics entity to the point
            physics_entity.position = to_bullet(transform.get_world_position());
            physics_entity.rotation = to_bullet(transform.get_world_rotation());

            // Calculate local inertia from the collision shape
            btVector3 local_inertia;
            physics_entity.collider.calculateLocalInertia(component.mass(), local_inertia);

            // Create the rigid body construction info
            btRigidBody::btRigidBodyConstructionInfo cinfo{
                component.mass(),
                &physics_entity,
                &physics_entity.collider,
                local_inertia };
            cinfo.m_friction = component.friction();
            cinfo.m_rollingFriction = component.rolling_friction();
            cinfo.m_spinningFriction = component.spinning_friction();
            cinfo.m_linearDamping = component.linear_damping();
            cinfo.m_angularDamping = component.angular_damping();

            // Create the rigid body
            physics_entity.rigid_body = std::make_unique<btRigidBody>(cinfo);

            // Add it to the world
            phys_world.dynamics_world().addRigidBody(physics_entity.rigid_body.get());
        }

        void BulletPhysicsSystem::Data::remove_rigid_body(EntityId entity)
        {
            auto& physics_entity = get_physics_entity(entity);
            assert(physics_entity.rigid_body != nullptr);

            // Remove the rigid body from the world
            phys_world.dynamics_world().removeRigidBody(physics_entity.rigid_body.get());

            // Delete the rigid body
            physics_entity.rigid_body = nullptr;

            post_modify_physics_entity(physics_entity);
        }

        PhysicsEntity& BulletPhysicsSystem::Data::get_or_create_physics_entity(EntityId entity)
        {
            // Search for the entity
            auto iter = physics_entities.find(entity);
            if (iter != physics_entities.end())
            {
                // Return the existing physics entity
                return *iter->second;
            }

            // Create a new physics entity
            auto phys = std::make_unique<PhysicsEntity>(entity, *this);
            return *physics_entities.insert(std::make_pair(entity, std::move(phys))).first->second;
        }

        PhysicsEntity& BulletPhysicsSystem::Data::get_physics_entity(EntityId entity)
        {
            auto iter = physics_entities.find(entity);
            assert(iter != physics_entities.end());
            return *iter->second;
        }

        void BulletPhysicsSystem::Data::post_modify_physics_entity(PhysicsEntity& phys_entity)
        {
            // If the physics entity has no child shapes and no rigid body
            if (phys_entity.collider.getNumChildShapes() == 0 && phys_entity.rigid_body == nullptr)
            {
                // ... Then there's no reason to keep it
                auto iter = physics_entities.find(phys_entity.entity);
                physics_entities.erase(iter);
            }
        }
    }
}
