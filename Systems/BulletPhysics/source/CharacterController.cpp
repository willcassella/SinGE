// CharacterController.cpp

#include <Engine/Components/Gameplay/CCharacterController.h>
#include <Core/Math/Mat4.h>
#include "../private/CharacterController.h"
#include "../private/PhysicsEntity.h"
#include "../private/Util.h"

namespace sge
{
    namespace bullet_physics
    {
        CharacterController::CharacterController(
            PhysicsEntity& phys_entity,
            const CCharacterController& component)
            : btKinematicCharacterController(
                nullptr,
                phys_entity.capsule_collider.get(),
                component.step_height(),
                btVector3{ 0, 1, 0 }),
            _phys_entity(&phys_entity)
        {
            // Initialize character controller properties
            setFallSpeed(component.fall_speed());
            setMaxSlope(component.max_slope().radians());
            setJumpSpeed(component.jump_speed());

            // Set ghost object
            ghost_object.setCollisionShape(phys_entity.capsule_collider.get());
            ghost_object.setCollisionFlags(ghost_object.getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);
            ghost_object.setWorldTransform(phys_entity.transform);
            m_ghostObject = &ghost_object;
        }

        void CharacterController::updateAction(btCollisionWorld* world, btScalar deltaTimeStep)
        {
            // Set walk direction
            setWalkDirection(to_bullet(_walk_dir.normalized()) / 10);

            // Set angular velocity
            setAngularVelocity(btVector3{ 0, _turn_amount.degrees(), 0 });

            // Call base implementation
            btKinematicCharacterController::updateAction(world, deltaTimeStep);

            // Update entity transform
            const auto& transform = getGhostObject()->getWorldTransform();
            _phys_entity->transform = transform;
            _phys_entity->add_to_modified();

            // Reset walk direction
            setWalkDirection(btVector3{ 0, 0, 0 });
            _walk_dir = Vec3::zero();

            // Reset turn amount
            setAngularVelocity(btVector3{ 0, 0, 0 });
            _turn_amount = 0;
        }

        void CharacterController::walk(Vec2 dir)
        {
            const Vec3 added_walk_dir{ dir.x(), 0, -dir.y() };
            _walk_dir += Mat4::rotate(from_bullet(_phys_entity->transform.getRotation())) * added_walk_dir;
        }

        void CharacterController::turn(Angle amount)
        {
            _turn_amount = _turn_amount + amount;
        }
    }
}
