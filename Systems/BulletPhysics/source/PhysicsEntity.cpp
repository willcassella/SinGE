// PhysicsEntity.cpp

#include "../private/PhysicsEntity.h"
#include "../private/CharacterController.h"
#include "../private/BulletPhysicsSystemData.h"

namespace sge
{
    namespace bullet_physics
    {
        PhysicsEntity::PhysicsEntity(EntityId entity, BulletPhysicsSystem::Data& data)
            : features(FT_NONE),
            entity(entity),
            collider(false),
            _data(&data)
        {
        }

        PhysicsEntity::~PhysicsEntity()
        {
        }

        void PhysicsEntity::getWorldTransform(btTransform& world_trans) const
        {
            world_trans = transform;
        }

        void PhysicsEntity::setWorldTransform(const btTransform& world_trans)
        {
            transform = world_trans;
            add_to_modified();
        }

        void PhysicsEntity::add_to_modified()
        {
            _data->frame_transformed_entities.insert(this);
        }

        void PhysicsEntity::extern_set_transform(const btTransform& trans)
        {
            // Set the main transform
            transform = trans;

            // Set the rigid body transform
            if (rigid_body)
            {
                rigid_body->setWorldTransform(trans);
                rigid_body->setInterpolationWorldTransform(trans);
            }

            // Set the ghost object trnasform
            if (ghost_object)
            {
                ghost_object->setWorldTransform(trans);
                ghost_object->setInterpolationWorldTransform(trans);
            }

            // Set the character controller transform
            if (character_controller)
            {
                character_controller->ghost_object.setWorldTransform(trans);
                character_controller->ghost_object.setInterpolationWorldTransform(trans);
            }
        }
    }
}
