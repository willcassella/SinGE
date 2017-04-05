// PhysicsEntity.cpp

#include "../private/PhysicsEntity.h"
#include "../private/CharacterController.h"
#include "../private/BulletPhysicsSystemData.h"
#include "../private/Util.h"

namespace sge
{
    namespace bullet_physics
    {
        PhysicsEntity::PhysicsEntity(NodeId node, BulletPhysicsSystem::Data& data)
            : node(node),
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
			PhysTransformedNode trans;
			trans.world_transform = from_bullet(transform.getOrigin());
			trans.world_rotation = from_bullet(transform.getRotation());

			_data->frame_transformed_nodes.push_back(node);
			_data->frame_transformed_node_transforms.push_back(trans);
        }

        void PhysicsEntity::extern_set_transform(const btTransform& trans, const btVector3& scale)
        {
            // Set the main transform
            transform = trans;
			collider.setLocalScaling(scale);

			if (collision_object)
			{
				collision_object->setWorldTransform(trans);
				collision_object->setInterpolationWorldTransform(trans);
			}
			if (ghost_object)
			{
				ghost_object->setWorldTransform(trans);
				ghost_object->setInterpolationWorldTransform(trans);
			}
            if (rigid_body)
            {
                rigid_body->setWorldTransform(trans);
                rigid_body->setInterpolationWorldTransform(trans);
            }
            if (character_controller)
            {
                character_controller->ghost_object.setWorldTransform(trans);
                character_controller->ghost_object.setInterpolationWorldTransform(trans);
            }
        }
    }
}
