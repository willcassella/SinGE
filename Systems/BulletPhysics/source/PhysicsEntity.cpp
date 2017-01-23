// PhysicsEntity.cpp

#include "../private/PhysicsEntity.h"
#include "../private/BulletPhysicsSystemData.h"

namespace sge
{
    namespace bullet_physics
    {
        PhysicsEntity::PhysicsEntity(EntityId entity, BulletPhysicsSystem::Data& data)
            : entity(entity),
            collider(false),
            _data(&data)
        {
        }

        void PhysicsEntity::getWorldTransform(btTransform& worldTrans) const
        {
            worldTrans.setOrigin(position);
            worldTrans.setRotation(rotation);
        }

        void PhysicsEntity::setWorldTransform(const btTransform& worldTrans)
        {
            position = worldTrans.getOrigin();
            rotation = worldTrans.getRotation();
            _data->frame_transformed_entities.push_back(this);
        }
    }
}
