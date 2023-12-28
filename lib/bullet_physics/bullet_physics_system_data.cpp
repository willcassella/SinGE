#include "lib/bullet_physics/bullet_physics_system_data.h"
#include "lib/bullet_physics/colliders.h"
#include "lib/bullet_physics/physics_entity.h"
#include "lib/bullet_physics/util.h"
#include "lib/resource/resources/static_mesh.h"

namespace sge {
namespace bullet_physics {
static std::unique_ptr<StaticMeshCollider>
create_static_mesh_collider_mesh(std::string path, const StaticMesh& mesh) {
  const auto num_verts = mesh.num_verts();
  const auto num_triangles = mesh.num_triangles();
  const auto* const vert_positions = mesh.vertex_positions();
  const auto* const triangle_elements = mesh.triangle_elements();

  // Build the mesh
  auto bt_mesh = std::make_unique<StaticMeshCollider>();
  bt_mesh->path = std::move(path);
  bt_mesh->mesh.preallocateVertices((int)num_verts);
  for (size_t i = 0; i < num_triangles; ++i) {
    bt_mesh->mesh.addTriangle(
        to_bullet(vert_positions[triangle_elements[i * 3 + 0]]),
        to_bullet(vert_positions[triangle_elements[i * 3 + 1]]),
        to_bullet(vert_positions[triangle_elements[i * 3 + 2]])
    );
  }
  bt_mesh->init_shape();

  return bt_mesh;
}

PhysicsEntity& BulletPhysicsSystem::Data::get_or_create_physics_entity(NodeId node_id, const Node& node) {
  // Search for the entity
  auto iter = physics_entities.find(node_id);
  if (iter != physics_entities.end()) {
    // Return the existing physics entity
    return *iter->second;
  }

  // Create a new physics entity
  auto phys = std::make_unique<PhysicsEntity>(node_id, *this);
  auto* phys_entity_ptr = phys.get();
  physics_entities.insert(std::make_pair(node_id, std::move(phys)));

  // Set the transform
  phys_entity_ptr->transform.setOrigin(to_bullet(node.get_local_position()));
  phys_entity_ptr->transform.setRotation(to_bullet(node.get_local_rotation()));
  phys_entity_ptr->collider.setLocalScaling(to_bullet(node.get_local_scale()));

  return *phys_entity_ptr;
}

PhysicsEntity* BulletPhysicsSystem::Data::get_physics_entity(NodeId node) {
  auto iter = physics_entities.find(node);
  return iter == physics_entities.end() ? nullptr : iter->second.get();
}

void BulletPhysicsSystem::Data::post_add_physics_entity_element(PhysicsEntity& phys_entity) {
  // If the physics entity does not have a ghost object, rigid body, or character controller, give it a basic
  // collision object
  if (phys_entity.lightmask_volume_ghost == nullptr && phys_entity.level_portal_ghost == nullptr &&
      phys_entity.rigid_body == nullptr && phys_entity.character_controller == nullptr) {
    if (phys_entity.collision_object != nullptr) {
      return;
    }

    // Create the collision object
    phys_entity.collision_object = std::make_unique<btCollisionObject>();
    phys_entity.collision_object->setCollisionShape(&phys_entity.collider);
    phys_entity.collision_object->setWorldTransform(phys_entity.transform);
    phys_entity.collision_object->setInterpolationWorldTransform(phys_entity.transform);
    phys_entity.collision_object->setUserIndex(phys_entity.get_user_index_1());
    phys_entity.collision_object->setUserIndex2(phys_entity.get_user_index_2());
    phys_entity.collision_object->setUserPointer(&phys_entity);

    // Add it to the world
    phys_world.dynamics_world().addCollisionObject(phys_entity.collision_object.get());
  } else {
    if (phys_entity.collision_object == nullptr) {
      return;
    }

    // Remove the collision object from the world, and destroy it
    phys_world.dynamics_world().removeCollisionObject(phys_entity.collision_object.get());
    phys_entity.collision_object = nullptr;
  }
}

void BulletPhysicsSystem::Data::post_remove_physics_entity_element(PhysicsEntity& phys_entity) {
  // If the physics entity has no rigid body, no ghost object, and no character controller
  if (phys_entity.rigid_body == nullptr && phys_entity.level_portal_ghost == nullptr &&
      phys_entity.lightmask_volume_ghost == nullptr && phys_entity.character_controller == nullptr) {
    // If it has child shapes, then we can't remove it (evaluate if we need to add a collision object)
    if (phys_entity.collider.getNumChildShapes() != 0) {
      if (phys_entity.collision_object != nullptr) {
        return;
      }

      // Create a collision object, and add it to the world
      phys_entity.collision_object = std::make_unique<btCollisionObject>();
      phys_entity.collision_object->setCollisionShape(&phys_entity.collider);
      phys_entity.collision_object->setWorldTransform(phys_entity.transform);
      phys_entity.collision_object->setInterpolationWorldTransform(phys_entity.transform);
      phys_entity.collision_object->setUserIndex(phys_entity.get_user_index_1());
      phys_entity.collision_object->setUserIndex2(phys_entity.get_user_index_2());
      phys_entity.collision_object->setUserPointer(&phys_entity);
      phys_world.dynamics_world().addCollisionObject(phys_entity.collision_object.get());
      return;
    }

    // Remove the collision object, if any
    if (phys_entity.collision_object != nullptr) {
      phys_world.dynamics_world().removeCollisionObject(phys_entity.collision_object.get());
    }

    // Destroy it
    auto iter = physics_entities.find(phys_entity.node);
    physics_entities.erase(iter);
  }
}

StaticMeshCollider* BulletPhysicsSystem::Data::get_static_mesh_collider(const std::string& path) {
  const auto iter = static_mesh_colliders.find(path);
  if (iter != static_mesh_colliders.end()) {
    auto* const ptr = iter->second.get();
    ptr->num_uses += 1;
    return ptr;
  }

  // Load the mesh
  StaticMesh mesh;
  if (!mesh.from_file(path.c_str())) {
    return nullptr;
  }

  // Create the collider
  auto collider = create_static_mesh_collider_mesh(path, mesh);
  auto* const ptr = collider.get();

  // Insert it into the table
  static_mesh_colliders.insert(std::make_pair(path, std::move(collider)));
  ptr->num_uses += 1;
  return ptr;
}

void BulletPhysicsSystem::Data::release_static_mesh_collider(StaticMeshCollider& collider) {
  collider.num_uses -= 1;
  if (collider.num_uses > 0) {
    return;
  }

  // Remove it from the table
  const auto path = collider.path;
  static_mesh_colliders.erase(path);
}
}  // namespace bullet_physics
}  // namespace sge
