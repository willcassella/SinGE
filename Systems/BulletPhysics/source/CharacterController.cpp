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
                phys_entity.ghost_object.get(),
                phys_entity.capsule_collider.get(),
                component.step_height(),
                btVector3{ 0, 1, 0 }),
            _phys_entity(&phys_entity)
        {
            // Initialize character controller properties
            setFallSpeed(component.fall_speed());
            setMaxSlope(component.max_slope().radians());
            setJumpSpeed(component.jump_speed());

            // Reset transform of ghost object (btKinematicCharacterController constructor messes it up)
            getGhostObject()->setWorldTransform(phys_entity.transform);
        }

        void CharacterController::updateAction(btCollisionWorld* world, btScalar deltaTimeStep)
        {
            // Set walk direction
            setWalkDirection(to_bullet(walk_dir.normalized()) / 10);

            // Call base implementation
            btKinematicCharacterController::updateAction(world, deltaTimeStep);

            // Update entity transform
            const auto& transform = getGhostObject()->getWorldTransform();
            _phys_entity->transform = transform;
            _phys_entity->add_to_modified();

            // Reset walk direction
            setWalkDirection(btVector3{ 0, 0, 0 });
            walk_dir = Vec3::zero();
        }

        void CharacterController::walk(Vec2 dir)
        {
            const Vec3 added_walk_dir{ dir.x(), 0, -dir.y() };
            walk_dir += Mat4::rotate(from_bullet(_phys_entity->transform.getRotation())) * added_walk_dir;
        }
    }
}
