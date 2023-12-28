#include <stdint.h>

#include "lib/bullet_physics/bullet_physics_system_data.h"
#include "lib/bullet_physics/level_portal.h"
#include "lib/bullet_physics/physics_entity.h"
#include "lib/engine/scene.h"

namespace sge
{
    namespace bullet_physics
    {
        void on_level_portal_new(
            EventChannel& new_level_portal_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data,
            Scene& scene)
        {
            // Get events
            ENewComponent events[8];
            int32_t num_events;
            while (new_level_portal_channel.consume(subscriber_id, events, &num_events))
            {
                // Get nodes
                NodeId node_ids[8];
                for (int32_t i = 0; i < num_events; ++i)
                {
                    node_ids[i] = events[i].node;
                }

                const Node* nodes[8];
                scene.get_nodes(node_ids, num_events, nodes);

                // Process events
                for (int32_t i = 0; i < num_events; ++i)
                {
                    auto& phys_entity = phys_data.get_or_create_physics_entity(node_ids[i], *nodes[i]);
                    assert(phys_entity.level_portal_ghost == nullptr);

                    // Create a ghost object for it
                    phys_entity.level_portal_ghost = std::make_unique<btPairCachingGhostObject>();
                    phys_entity.level_portal_ghost->setCollisionShape(&phys_entity.collider);
                    phys_entity.level_portal_ghost->setWorldTransform(phys_entity.transform);
                    phys_entity.level_portal_ghost->setInterpolationWorldTransform(phys_entity.transform);
                    phys_entity.level_portal_ghost->setUserPointer(&phys_entity);
                    phys_entity.level_portal_ghost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
                    phys_data.phys_world.dynamics_world().addCollisionObject(phys_entity.level_portal_ghost.get());
                    phys_data.post_add_physics_entity_element(phys_entity);

                    phys_entity.set_user_index_1(phys_entity.get_user_index_1() | LEVEL_PORTAL_BIT);
                }
            }
        }

        void on_level_portal_destroyed(
            EventChannel& destroyed_level_portal_channel,
            EventChannel::SubscriberId subscriber_id,
            BulletPhysicsSystem::Data& phys_data)
        {
            // Get events
            EDestroyedComponent events[8];
            int32_t num_events;
            while (destroyed_level_portal_channel.consume(subscriber_id, events, &num_events))
            {
                // Process events
                for (int32_t i = 0; i < num_events; ++i)
                {
                    auto* phys_entity = phys_data.get_physics_entity(events[i].node);
                    if (!phys_entity)
                    {
                        continue;
                    }

                    // Remove the ghost object
                    if (phys_entity->level_portal_ghost)
                    {
                        phys_data.phys_world.dynamics_world().removeCollisionObject(phys_entity->level_portal_ghost.get());
                        phys_entity->level_portal_ghost = nullptr;
                    }

                    phys_entity->set_user_index_1(phys_entity->get_user_index_1() & ~LEVEL_PORTAL_BIT);
                    phys_data.post_remove_physics_entity_element(*phys_entity);
                }
            }
        }
    }
}
