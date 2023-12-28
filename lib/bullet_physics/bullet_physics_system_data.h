#pragma once

#include <stdint.h>
#include <map>

#include "lib/bullet_physics/bullet_physics_system.h"
#include "lib/bullet_physics/physics_world.h"
#include "lib/engine/component.h"

namespace sge {
struct CBoxCollider;
struct CCapsuleCollider;
struct CRigidBody;
struct CCharacterController;

namespace bullet_physics {
class PhysicsEntity;
class CharacterController;
struct StaticMeshCollider;

struct PhysTransformedNode {
  Vec3 world_transform;
  Quat world_rotation;
};

// Bits used for efficiently identifying collider archetypes
static constexpr int CHARACTER_BIT = 1;
static constexpr int LEVEL_PORTAL_BIT = 2;
static constexpr int LIGHTMASK_RECEIVER_BIT = 4;
static constexpr int LIGHTMASK_VOLUME_BIT = 8;

struct BulletPhysicsSystem::Data {
  PhysicsEntity& get_or_create_physics_entity(NodeId node_id, const Node& node);

  PhysicsEntity* get_physics_entity(NodeId node);

  void post_add_physics_entity_element(PhysicsEntity& phys_entity);

  void post_remove_physics_entity_element(PhysicsEntity& phys_entity);

  StaticMeshCollider* get_static_mesh_collider(const std::string& path);

  void release_static_mesh_collider(StaticMeshCollider& collider);

  uint64_t last_frame_id = 0;

  std::map<NodeId, std::unique_ptr<PhysicsEntity>> physics_entities;

  // Nodes that were transformed this frame (by the pysics system), and how they were transformed
  std::vector<NodeId> frame_transformed_nodes;
  std::vector<PhysTransformedNode> frame_transformed_node_transforms;
  std::map<std::string, std::unique_ptr<StaticMeshCollider>> static_mesh_colliders;

  /* NOTE: This must appear last, so that it is destroyed first. */
  PhysicsWorld phys_world;
};
}  // namespace bullet_physics
}  // namespace sge
