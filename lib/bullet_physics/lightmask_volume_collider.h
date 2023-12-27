#pragma once

#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>

#include "lib/bullet_physics/bullet_physics_system.h"
#include "lib/engine/components/display/spot_light.h"

namespace sge
{
    namespace bullet_physics
    {
        struct LightmaskVolumeCollider
        {
            ~LightmaskVolumeCollider()
            {
                get_mesh_shape()->~btBvhTriangleMeshShape();
            }

            void init_shape()
            {
                new (_buffer) btBvhTriangleMeshShape{ &mesh, true };
            }

            btBvhTriangleMeshShape* get_mesh_shape()
            {
                return reinterpret_cast<btBvhTriangleMeshShape*>(_buffer);
            }

            /* The shape itself. */
            btTriangleMesh mesh;

        private:
            alignas(alignof(btBvhTriangleMeshShape))
            char _buffer[sizeof(btBvhTriangleMeshShape)];
        };

        void on_spotlight_new(
            EventChannel& new_spotlight_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data,
            Scene& scene);

        void on_spotlight_destroyed(
            EventChannel& destroyed_spotlight_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data);
    }
}
