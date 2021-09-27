// Colliders.h
#pragma once

#include "BulletPhysicsSystemData.h"

namespace sge
{
    namespace bullet_physics
    {
        struct StaticMeshCollider
        {
            ////////////////////////
            ///   Constructors   ///
        public:

            ~StaticMeshCollider()
            {
                get_mesh_shape()->~btBvhTriangleMeshShape();
            }

            ///////////////////
            ///   Methods   ///
        public:

            void init_shape()
            {
                new (_buffer) btBvhTriangleMeshShape{ &mesh, true };
            }

            btBvhTriangleMeshShape* get_mesh_shape()
            {
                return reinterpret_cast<btBvhTriangleMeshShape*>(_buffer);
            }

            //////////////////
            ///   Fields   ///
        public:

            /* Used to relocate the unique_ptr in the table when num_uses == 0 */
            std::string path;

            /* The number of uses of this collider. */
            uint64 num_uses = 0;

            /* The shape itself. */
            btTriangleMesh mesh;

        private:

            alignas(alignof(btBvhTriangleMeshShape))
            char _buffer[sizeof(btBvhTriangleMeshShape)];
        };

        void on_sphere_collider_new(
            EventChannel& new_sphere_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data,
            Scene& scene);

        void on_sphere_collider_destroyed(
            EventChannel& destroyed_sphere_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data);

        void on_sphere_collider_modified(
            EventChannel& modified_sphere_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data);

        void on_box_collider_new(
            EventChannel& new_box_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data,
            Scene& scene);

        void on_box_collider_destroyed(
            EventChannel& destroyed_box_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data);

        void on_box_collider_modified(
            EventChannel& modified_box_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data);

        void on_capsule_collider_new(
            EventChannel& new_capsule_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data,
            Scene& scene);

        void on_capsule_collider_destroyed(
            EventChannel& destroyed_capsule_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data);

        void on_capsule_collider_modified(
            EventChannel& modifed_capsule_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data);

        void on_static_mesh_collider_new(
            EventChannel& new_static_mesh_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data,
            Scene& scene);

        void on_static_mesh_collider_destroyed(
            EventChannel& destroyed_static_mesh_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data);

        void on_static_mesh_collider_modified(
            EventChannel& modified_static_mesh_collider_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data,
            Scene& scene);
    }
}
