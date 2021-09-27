// Spotlight.h
#pragma once

#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <Engine/Components/Display/CSpotlight.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"

namespace sge
{
    namespace bullet_physics
    {
        struct LightmaskVolumeCollider
        {
            /*--- Constructors ---*/
        public:

            ~LightmaskVolumeCollider()
            {
                get_mesh_shape()->~btBvhTriangleMeshShape();
            }

            /*--- Methods ---*/
        public:

            void init_shape()
            {
                new (_buffer) btBvhTriangleMeshShape{ &mesh, true };
            }

            btBvhTriangleMeshShape* get_mesh_shape()
            {
                return reinterpret_cast<btBvhTriangleMeshShape*>(_buffer);
            }

            /*--- Fields ---*/
        public:

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
