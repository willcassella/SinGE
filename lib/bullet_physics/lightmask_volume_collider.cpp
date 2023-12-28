#include <stdint.h>

#include <btBulletCollisionCommon.h>

#include "lib/bullet_physics/bullet_physics_system_data.h"
#include "lib/bullet_physics/lightmask_volume_collider.h"
#include "lib/bullet_physics/physics_entity.h"
#include "lib/bullet_physics/util.h"
#include "lib/engine/scene.h"
#include "lib/resource/misc/lightmask_volume.h"

namespace sge {
namespace bullet_physics {
static std::unique_ptr<LightmaskVolumeCollider> build_lightmask_volume_collider(const CSpotlight& component) {
  // Build the mesh for the collider
  Vec3 frustum_verts[NUM_FRUSTUM_VERTS];
  uint32_t frustum_elems[NUM_FRUSTUM_ELEMS];
  create_lightmask_volume_frustum_positions(
      component.near_clipping_plane(),
      component.far_clipping_plane(),
      component.frustum_horiz_angle().radians(),
      component.frustum_vert_angle().radians(),
      frustum_verts
  );
  create_lightmask_volume_frustum_elems(frustum_elems);

  auto mesh = std::make_unique<LightmaskVolumeCollider>();
  mesh->mesh.preallocateVertices(NUM_FRUSTUM_ELEMS);
  for (size_t triangle_i = 0; triangle_i < NUM_FRUSTUM_ELEMS / 3; ++triangle_i) {
    mesh->mesh.addTriangle(
        to_bullet(frustum_verts[frustum_elems[triangle_i * 3 + 0]]),
        to_bullet(frustum_verts[frustum_elems[triangle_i * 3 + 1]]),
        to_bullet(frustum_verts[frustum_elems[triangle_i * 3 + 2]])
    );
  }
  mesh->init_shape();

  return mesh;
}

void on_spotlight_new(
    EventChannel& new_spotlight_channel,
    EventChannel::SubscriberId subscriber_id,
    BulletPhysicsSystem::Data& phys_data,
    Scene& scene
) {
  // Get events
  ENewComponent events[8];
  int32_t num_events;
  while (new_spotlight_channel.consume(subscriber_id, events, &num_events)) {
    // Get nodes
    NodeId node_ids[8];
    const CSpotlight* components[8];
    for (int32_t i = 0; i < num_events; ++i) {
      node_ids[i] = events[i].node;
      components[i] = (const CSpotlight*)events[i].instance;
    }

    const Node* nodes[8];
    scene.get_nodes(node_ids, num_events, nodes);

    // Process events
    for (int32_t i = 0; i < num_events; ++i) {
      if (!components[i]->is_lightmask_volume() || components[i]->shape() != CSpotlight::Shape::FRUSTUM) {
        continue;
      }

      auto& phys_entity = phys_data.get_or_create_physics_entity(node_ids[i], *nodes[i]);
      assert(phys_entity.lightmask_volume_ghost == nullptr);

      // Create the collider
      auto collider = build_lightmask_volume_collider(*components[i]);
      collider->get_mesh_shape()->setLocalScaling(phys_entity.collider.getLocalScaling());

      // Create the ghost object
      phys_entity.lightmask_volume_ghost = std::make_unique<btPairCachingGhostObject>();
      phys_entity.lightmask_volume_ghost->setWorldTransform(phys_entity.transform);
      phys_entity.lightmask_volume_ghost->setInterpolationWorldTransform(phys_entity.transform);
      phys_entity.lightmask_volume_ghost->setUserPointer(&phys_entity);
      phys_entity.lightmask_volume_ghost->setCollisionShape(collider->get_mesh_shape());
      phys_entity.lightmask_volume_ghost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
      phys_data.phys_world.dynamics_world().addCollisionObject(
          phys_entity.lightmask_volume_ghost.get(),
          btBroadphaseProxy::SensorTrigger,
          btBroadphaseProxy::CharacterFilter
      );
      phys_data.post_add_physics_entity_element(phys_entity);

      // Add the collider to the object, and set the proper bit on it
      phys_entity.lightmask_volume_collider = std::move(collider);
      phys_entity.lightmask_volume_ghost->setUserIndex(
          phys_entity.lightmask_volume_ghost->getUserIndex() & LIGHTMASK_VOLUME_BIT
      );
      phys_data.post_add_physics_entity_element(phys_entity);
    }
  }
}

void on_spotlight_destroyed(
    EventChannel& destroyed_spotlight_channel,
    EventChannel::SubscriberId subscriber_id,
    BulletPhysicsSystem::Data& phys_data
) {
  // Get events
  EDestroyedComponent events[8];
  int32_t num_events;
  while (destroyed_spotlight_channel.consume(subscriber_id, events, &num_events)) {
    for (int32_t i = 0; i < num_events; ++i) {
      const NodeId node = events[i].node;

      auto* phys_entity = phys_data.get_physics_entity(node);
      if (!phys_entity || !phys_entity->lightmask_volume_collider) {
        continue;
      }

      // Destroy the ghost object and collider
      phys_entity->lightmask_volume_ghost = nullptr;
      phys_entity->lightmask_volume_collider = nullptr;

      // Evaluate if we should keep the physics entity
      phys_data.post_remove_physics_entity_element(*phys_entity);
    }
  }
}
}  // namespace bullet_physics
}  // namespace sge
