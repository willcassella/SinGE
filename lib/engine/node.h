#pragma once

#include <inttypes.h>
#include <stdint.h>
#include <vector>

#include "lib/base/math/mat4.h"
#include "lib/engine/event_channel.h"

namespace sge {
struct Scene;
struct NodeLocalTransformMod;
struct NodeRootMod;

struct SGE_ENGINE_API NodeId {
  SGE_REFLECTED_TYPE;
  using Version_t = uint32_t;
  using Index_t = uint32_t;

  NodeId() : index(0) {}

  explicit NodeId(uint64_t value) { from_u64(value); }

  static NodeId null_id() { return NodeId{0}; }

  uint64_t to_u64() const { return index; }

  void from_u64(uint64_t value) { index = value & 0x00000000FFFFFFFF; }

  size_t to_string(char* out_str, size_t size) const {
    return (size_t)snprintf(out_str, size, "%" PRIu64, to_u64());
  }

  void from_string(const char* str) {
    const uint64_t value = strtoull(str, nullptr, 10);
    from_u64(value);
  }

  void to_archive(ArchiveWriter& writer) const { writer.number(to_u64()); }

  void from_archive(ArchiveReader& reader) {
    uint64_t value = 0;
    reader.number(value);
    from_u64(value);
  }

  bool is_null() const { return index == 0; }

  friend bool operator==(const NodeId& lhs, const NodeId& rhs) { return lhs.index == rhs.index; }
  friend bool operator!=(const NodeId& lhs, const NodeId& rhs) { return !(lhs == rhs); }
  friend bool operator<(const NodeId& lhs, const NodeId& rhs) { return lhs.index < rhs.index; }

  Index_t index;
};

struct SGE_ENGINE_API Node {
  SGE_REFLECTED_TYPE;
  friend Scene;

  using ModState_t = uint32_t;
  enum ModState : ModState_t {
    /**
     * \brief This node has not been modified.
     */
    NONE = 0,

    /**
     * \brief This node's root has been modified, but the change has not yet been applied.
     */
    ROOT_PENDING = 1 << 0,

    /**
     * \brief This node's root has been modified during the frame, and the update has been applied to the
     * scene.
     */
    ROOT_APPLIED = 1 << 1,

    /**
     * \brief This node's local transform has been modified, and the update is pending.
     */
    TRANSFORM_PENDING = 1 << 2,

    /**
     * \brief This node's local transform has been modified during the frame, and the update has been applied.
     */
    TRANSFORM_APPLIED = 1 << 3,

    /**
     * \brief This node has been created this frame.
     */
    NEW = 1 << 4,

    /**
     * \brief This node has been marked for destruction during this system frame.
     */
    DESTROYED_PENDING = 1 << 5,

    /**
     * \brief This node (and all of its children) have been marked for destruction during this system frame.
     */
    DESTROYED_APPLIED = 1 << 6,

    /**
     * \brief Helper element, represents bits used when a node has been modified.
     */
    H_NODE_MODIFIED = ~(DESTROYED_PENDING | DESTROYED_APPLIED)
  };

  /**
   * \brief Returns the Id for this node.
   */
  NodeId get_id() const;

  /**
   * \brief Returns the name of this node.
   */
  const std::string& get_name() const;

  /**
   * \brief Sets the name of this node.
   */
  void set_name(std::string name);

  /**
   * \brief Returns the current modification state for this node.
   */
  ModState_t get_mod_state() const;

  /**
   * \brief Returns the Id of the root for this node.
   */
  NodeId get_root() const;

  /**
   * \brief Sets the pending root of this node.
   * \param root The pending root to set. Use nullptr to set no root.
   */
  void set_root(Node* root);

  /**
   * \brief Returns the pending root of this node.
   * NOTE: If there is no pending root, this returns the current root.
   */
  NodeId get_pending_root() const;

  /**
   * \brief Returns the current number of children of this node.
   */
  size_t get_num_children() const;

  /**
   * \brief Fills the given array with the Ids of all children of this node betwee [start_index, start_index +
   * num_children) \param start_index The index of the first child to retreive. \param num_children The number
   * of children to retreive. \param out_num_children Variable to assign with the number of children
   * retrieved. \param out_children The array to fill with the ids of the children. \return The number of
   * children retreived.
   */
  size_t get_children(size_t start_index, size_t num_children, size_t* out_num_children, NodeId* out_children)
      const;

  /**
   * \brief Sets this node as the pending root of the given node.
   * \param child The child to set.
   */
  void add_child(Node& child);

  /**
   * \brief If the given node is a child of this node, sets its pending root as NULL.
   * \param child The child to remove.
   */
  void remove_child(Node& child);

  /**
   * \brief Returns the position of this node relative to the root.
   */
  Vec3 get_local_position() const;

  /**
   * \brief Sets the pending local position of this node.
   * \param pos The local position to set.
   */
  void set_local_position(Vec3 pos);

  /**
   * \brief Returns the pending local position of this node.
   * NOTE: If there is no current pending local position, this returns the current local position.
   */
  Vec3 get_pending_local_position() const;

  /**
   * \brief Returns the scale of this node relative to the root.
   */
  Vec3 get_local_scale() const;

  /**
   * \brief Sets the pending local scale of this node.
   * \param scale The local scale to set.
   */
  void set_local_scale(Vec3 scale);

  /**
   * \brief Returns the pending local scale of this node.
   * NOTE: If there is no current pending local scale, this returns the current local scale.
   */
  Vec3 get_pending_local_scale() const;

  /**
   * \brief Returns the local rotation of this node relative to the root.
   */
  Quat get_local_rotation() const;

  /**
   * \brief Sets the pending local rotation of this node.
   * \param rot The local rotation to set.
   */
  void set_local_rotation(Quat rot);

  /**
   * \brief Returns the pending local rotation of this node.
   * NOTE: If there is no pending local rotation, this returns the current local rotation.
   */
  Quat get_pending_local_rotation() const;

  /**
   * \brief Returns the local-to-world matrix for this node.
   */
  const Mat4& get_world_matrix() const;

 private:
  Node();

  static bool sort_by_hierarchy_depth(const Node* lhs, const Node* rhs);

  NodeLocalTransformMod& get_or_create_transform_mod();

  NodeRootMod& get_or_create_root_mod();

  Scene* _scene;
  NodeId _id;
  NodeId _root;
  std::vector<NodeId> _child_nodes;
  uint32_t _hierarchy_depth;
  ModState_t _mod_state;
  int32_t _transform_mod_index;
  int32_t _root_mod_index;
  Vec3 _local_position;
  Vec3 _local_scale;
  Quat _local_rotation;
  Mat4 _cached_world_matrix;
  std::string _name;
};

struct ENewNode {
  Node* node;
};

struct EDestroyedNode {
  Node* node;
};

struct ENodeTransformChanged {
  Node* node;
};

struct ENodeRootChangd {
  Node* node;
  Node* root;
};
}  // namespace sge
