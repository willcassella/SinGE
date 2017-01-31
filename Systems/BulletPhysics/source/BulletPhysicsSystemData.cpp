// BulletPhysicsSystemData.cpp

#include <Engine/Components/Physics/CBoxCollider.h>
#include <Engine/Components/Physics/CCapsuleCollider.h>
#include <Engine/Components/Physics/CRigidBody.h>
#include <Engine/Components/CTransform3D.h>
#include "../private/BulletPhysicsSystemData.h"
#include "../private/PhysicsEntity.h"
#include "../private/CharacterController.h"
#include "../private/Util.h"

namespace sge
{
    namespace bullet_physics
    {
        bool PhysicsEntityLess::operator()(const PhysicsEntity* lhs, const PhysicsEntity* rhs)
        {
            return lhs->entity < rhs->entity;
        }

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

            // Remove the box collider from the compound and the physisc entity
            phys_entity.collider.removeChildShape(phys_entity.box_collider.get());
            phys_entity.box_collider = nullptr;

            post_modify_physics_entity(phys_entity);
        }

        void BulletPhysicsSystem::Data::add_capsule_collider(EntityId entity, const CCapsuleCollider& component)
        {
            auto& physics_entity = get_or_create_physics_entity(entity);
            assert(physics_entity.capsule_collider == nullptr);
            physics_entity.capsule_collider = std::make_unique<btCapsuleShape>(component.radius(), component.height());
            physics_entity.collider.addChildShape(btTransform::getIdentity(), physics_entity.capsule_collider.get());
        }

        void BulletPhysicsSystem::Data::remove_capsule_collider(EntityId entity)
        {
            auto& physics_entity = get_physics_entity(entity);
            assert(physics_entity.capsule_collider != nullptr);

            // Remove the capsule colider from the compound and the physics entity
            physics_entity.collider.removeChildShape(physics_entity.capsule_collider.get());
            physics_entity.capsule_collider = nullptr;

            post_modify_physics_entity(physics_entity);
        }

        void BulletPhysicsSystem::Data::add_rigid_body(EntityId entity, const CTransform3D& transform, const CRigidBody& component)
        {
            auto& physics_entity = get_or_create_physics_entity(entity);
            assert(physics_entity.rigid_body == nullptr);

            // Set the entitiy's position
            to_bullet(physics_entity.transform, transform);

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

            // Set if it's kinematic or not
            if (component.kinematic())
            {
                physics_entity.rigid_body->setMassProps(0, { 0, 0, 0 });
                physics_entity.rigid_body->setActivationState(DISABLE_DEACTIVATION);
                physics_entity.rigid_body->setCollisionFlags(
                    physics_entity.rigid_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            }

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

        void BulletPhysicsSystem::Data::add_character_controller(
            EntityId entity,
            const CTransform3D& transform,
            const CCharacterController& character_controller)
        {
            auto& physics_entity = get_physics_entity(entity);
            assert(physics_entity.character_controller == nullptr);

            // Set the transform of the entity
            to_bullet(physics_entity.transform, transform);

            // Create the character controller
            physics_entity.character_controller = std::make_unique<CharacterController>(physics_entity, character_controller);

            // Add the ghost object to the world
            phys_world.dynamics_world().addCollisionObject(&physics_entity.character_controller->ghost_object,
                btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

            // Add the character controller to the world
            phys_world.dynamics_world().addAction(physics_entity.character_controller.get());
        }

        void BulletPhysicsSystem::Data::remove_character_contoller(EntityId entity)
        {
            auto& phys_entity = get_physics_entity(entity);
            assert(phys_entity.character_controller != nullptr);

            // Remove it from the world
            phys_world.dynamics_world().removeAction(phys_entity.character_controller.get());
            phys_world.dynamics_world().removeCollisionObject(&phys_entity.character_controller->ghost_object);
            phys_entity.character_controller = nullptr;
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
