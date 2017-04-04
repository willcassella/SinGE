// BulletPhysicsSystemData.cpp

#include <Engine/Components/Physics/CBoxCollider.h>
#include <Engine/Components/Physics/CCapsuleCollider.h>
#include <Engine/Components/Physics/CRigidBody.h>
#include "../private/BulletPhysicsSystemData.h"
#include "../private/PhysicsEntity.h"
#include "../private/CharacterController.h"
#include "../private/Util.h"

namespace sge
{
    namespace bullet_physics
	{
        void BulletPhysicsSystem::Data::add_rigid_body(const Node& node, const CRigidBody& rigid_body)
        {
            auto& physics_entity = get_or_create_physics_entity(node.get_id());
            assert(physics_entity.rigid_body == nullptr);

            // Set the entitiy's position
			physics_entity.transform.setOrigin(to_bullet(node.get_local_position()));
			physics_entity.transform.setRotation(to_bullet(node.get_local_rotation()));

            // Calculate local inertia from the collision shape
            btVector3 local_inertia;
            physics_entity.collider.calculateLocalInertia(rigid_body.mass(), local_inertia);

            // Create the rigid body construction info
            btRigidBody::btRigidBodyConstructionInfo cinfo{
                rigid_body.mass(),
                &physics_entity,
                &physics_entity.collider,
                local_inertia };
            cinfo.m_friction = rigid_body.friction();
            cinfo.m_rollingFriction = rigid_body.rolling_friction();
            cinfo.m_spinningFriction = rigid_body.spinning_friction();
            cinfo.m_linearDamping = rigid_body.linear_damping();
            cinfo.m_angularDamping = rigid_body.angular_damping();

            // Create the rigid body
            physics_entity.rigid_body = std::make_unique<btRigidBody>(cinfo);

            // Set if it's kinematic or not
            if (rigid_body.kinematic())
            {
                physics_entity.rigid_body->setMassProps(0, { 0, 0, 0 });
                physics_entity.rigid_body->setActivationState(DISABLE_DEACTIVATION);
                physics_entity.rigid_body->setCollisionFlags(
                    physics_entity.rigid_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            }

            // Add it to the world
            phys_world.dynamics_world().addRigidBody(physics_entity.rigid_body.get());
        }

        void BulletPhysicsSystem::Data::remove_rigid_body(NodeId node)
        {
            auto* physics_entity = get_physics_entity(node);
            assert(physics_entity != nullptr && physics_entity->rigid_body != nullptr);

            // Remove the rigid body from the world
            phys_world.dynamics_world().removeRigidBody(physics_entity->rigid_body.get());

            // Delete the rigid body
            physics_entity->rigid_body = nullptr;

            post_remove_physics_entity_element(*physics_entity);
        }

        void BulletPhysicsSystem::Data::add_character_controller(
            const Node& node,
            const CCharacterController& character_controller)
        {
            auto& physics_entity = get_or_create_physics_entity(node.get_id());
            assert(physics_entity.character_controller == nullptr);

            // Set the transform of the entity
			physics_entity.transform.setOrigin(to_bullet(node.get_local_position()));
			physics_entity.transform.setRotation(to_bullet(node.get_local_rotation()));

            // Create the character controller
            physics_entity.character_controller = std::make_unique<CharacterController>(physics_entity, character_controller);

            // Add the ghost object to the world
            phys_world.dynamics_world().addCollisionObject(&physics_entity.character_controller->ghost_object,
                btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

            // Add the character controller to the world
            phys_world.dynamics_world().addAction(physics_entity.character_controller.get());
        }

        void BulletPhysicsSystem::Data::remove_character_contoller(NodeId node)
        {
            auto* phys_entity = get_physics_entity(node);
            assert(phys_entity != nullptr && phys_entity->character_controller != nullptr);

            // Remove it from the world
            phys_world.dynamics_world().removeAction(phys_entity->character_controller.get());
            phys_world.dynamics_world().removeCollisionObject(&phys_entity->character_controller->ghost_object);
            phys_entity->character_controller = nullptr;

			post_remove_physics_entity_element(*phys_entity);
        }

        PhysicsEntity& BulletPhysicsSystem::Data::get_or_create_physics_entity(NodeId node)
        {
            // Search for the entity
            auto iter = physics_entities.find(node);
            if (iter != physics_entities.end())
            {
                // Return the existing physics entity
                return *iter->second;
            }

            // Create a new physics entity
            auto phys = std::make_unique<PhysicsEntity>(node, *this);
            return *physics_entities.insert(std::make_pair(node, std::move(phys))).first->second;
        }

        PhysicsEntity* BulletPhysicsSystem::Data::get_physics_entity(NodeId node)
        {
            auto iter = physics_entities.find(node);
            return iter == physics_entities.end() ? nullptr : iter->second.get();
        }

        void BulletPhysicsSystem::Data::post_remove_physics_entity_element(PhysicsEntity& phys_entity)
        {
            // If the physics entity has no child shapes, no rigid body, no ghost object, and no character controller
            if (phys_entity.collider.getNumChildShapes() == 0 && phys_entity.rigid_body == nullptr && phys_entity.ghost_object == nullptr && phys_entity.character_controller == nullptr)
            {
                // ... Then there's no reason to keep it
                auto iter = physics_entities.find(phys_entity.node);
                physics_entities.erase(iter);
            }
        }
    }
}
