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
            _data->frame_transformed_entities.push_back(this);
        }
    }
}
