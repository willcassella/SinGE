#include "lib/bullet_physics/physics_entity.h"
#include "lib/bullet_physics/bullet_physics_system_data.h"
#include "lib/bullet_physics/character_controller.h"
#include "lib/bullet_physics/lightmask_volume_collider.h"
#include "lib/bullet_physics/util.h"

namespace sge {
namespace bullet_physics {
PhysicsEntity::PhysicsEntity(NodeId node, BulletPhysicsSystem::Data& data)
    : node(node), collider(false), phys_data(&data), _user_index_1_value(0), _user_index_2_value(0) {}

PhysicsEntity::~PhysicsEntity() {}

void PhysicsEntity::getWorldTransform(btTransform& world_trans) const {
  world_trans = transform;
}

void PhysicsEntity::setWorldTransform(const btTransform& world_trans) {
  transform = world_trans;

  if (lightmask_volume_ghost) {
    lightmask_volume_ghost->setWorldTransform(world_trans);
    lightmask_volume_ghost->setInterpolationWorldTransform(world_trans);
  }

  add_to_modified();
}

void PhysicsEntity::add_to_modified() {
  PhysTransformedNode trans;
  trans.world_transform = from_bullet(transform.getOrigin());
  trans.world_rotation = from_bullet(transform.getRotation());

  phys_data->frame_transformed_nodes.push_back(node);
  phys_data->frame_transformed_node_transforms.push_back(trans);
}

void PhysicsEntity::extern_set_transform(const btTransform& trans, const btVector3& scale) {
  // Set the main transform
  transform = trans;
  collider.setLocalScaling(scale);

  if (collision_object) {
    collision_object->setWorldTransform(trans);
    collision_object->setInterpolationWorldTransform(trans);
  }
  if (lightmask_volume_ghost) {
    lightmask_volume_ghost->setWorldTransform(trans);
    lightmask_volume_ghost->setInterpolationWorldTransform(trans);
  }
  if (level_portal_ghost) {
    level_portal_ghost->setWorldTransform(trans);
    level_portal_ghost->setInterpolationWorldTransform(trans);
  }
  if (rigid_body) {
    rigid_body->setWorldTransform(trans);
    rigid_body->setInterpolationWorldTransform(trans);
  }
  if (character_controller) {
    character_controller->ghost_object.setWorldTransform(trans);
    character_controller->ghost_object.setInterpolationWorldTransform(trans);
  }
}

int PhysicsEntity::get_user_index_1() const {
  return _user_index_1_value;
}

void PhysicsEntity::set_user_index_1(int value) {
  _user_index_1_value = value;

  if (collision_object) {
    collision_object->setUserIndex(value);
  }
  if (level_portal_ghost) {
    level_portal_ghost->setUserIndex(value);
  }
  if (rigid_body) {
    rigid_body->setUserIndex(value);
  }
  if (character_controller) {
    character_controller->ghost_object.setUserIndex(value);
  }
}

int PhysicsEntity::get_user_index_2() const {
  return _user_index_2_value;
}

void PhysicsEntity::set_user_index_2(int value) {
  _user_index_2_value = value;

  if (collision_object) {
    collision_object->setUserIndex2(value);
  }
  if (level_portal_ghost) {
    level_portal_ghost->setUserIndex2(value);
  }
  if (rigid_body) {
    rigid_body->setUserIndex2(value);
  }
  if (character_controller) {
    character_controller->ghost_object.setUserIndex2(value);
  }
}
}  // namespace bullet_physics
}  // namespace sge
