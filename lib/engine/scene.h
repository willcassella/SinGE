#pragma once

#include <stdint.h>

#include "lib/engine/scene_data.h"

namespace sge {
struct TypeDB;
struct UpdatePipeline;
struct SystemInfo;

/**
 * \brief Top-level scene interface.
 */
struct SGE_ENGINE_API Scene {
  SGE_REFLECTED_TYPE;
  friend SystemFrame;

  Scene(TypeDB& typedb);
  Scene(const Scene& copy) = delete;
  Scene& operator=(const Scene& copy) = delete;
  ~Scene();

  void create_nodes(size_t num_nodes, Node** out_nodes);

  void destroy_nodes(size_t num_nodes, Node* const* nodes);

  void get_nodes(const NodeId* nodes, size_t num_nodes, Node** out_nodes);

  void get_nodes(const NodeId* nodes, size_t num_nodes, const Node** out_nodes) const;

  size_t num_root_nodes() const;

  size_t get_root_nodes(size_t start_index, size_t num_nodes, size_t* out_num_nodes, NodeId* out_nodes) const;

  ComponentContainer* get_component_container(const TypeInfo& type);

  EventChannel* get_event_channel(const TypeInfo& component_type, const char* channel_name);

  EventChannel* get_node_local_transform_changed_channel();

  EventChannel* get_node_world_transform_changed_channel();

  EventChannel* get_node_root_changed_channel();

  EventChannel* get_debug_draw_line_channel();

  /**
   * \brief Resets node/component data. Essentially creates a new scene without unregistering component types.
   */
  void reset_scene();

  SceneData& get_raw_scene_data();

  /**
   * \brief Returns raw scene data. You should not use this unless you know what you're doing.
   */
  const SceneData& get_raw_scene_data() const;

  /**
   * \brief Serializes the state of this Scene to an Archive.
   * \param writer The writer for the archive to serialize to.
   */
  void to_archive(ArchiveWriter& writer) const;

  /**
   * \brief Deserializes the state of this Scene from an Archive.
   * \param reader The reader for the Archive to deserialize from.
   */
  void from_archive(ArchiveReader& reader);

  /**
   * \brief Returns the type database for this Scene.
   */
  TypeDB& get_type_db();

  /**
   * \brief Returns the type database for this Scene.
   */
  const TypeDB& get_type_db() const;

  /**
   * \brief Searches for a component type with the given name in the type database.
   * \param typeName The name of the component type to search for.
   * \return A pointer to the type information for the type, if found.
   */
  const TypeInfo* get_component_type(const char* typeName) const;

  /**
   * \brief Registers a new component type with the scene.
   * \param type
   * \param container
   */
  void register_component_type(const TypeInfo& type, std::unique_ptr<ComponentContainer> container);

  /**
   * \brief Runs a full update of the scene, using the given update pipeline and time delta.
   * \param pipeline The update pipeline to use to update the scene.
   * \param dt The game time that is supposed to have passed since the last call to 'update'.
   */
  void update(UpdatePipeline& pipeline, float dt);

 private:
  void initialize_hierarchy_depths();

  void
  execute_job_queue(SystemInfo* const* jobs, size_t num_jobs, UpdatePipeline& pipeline, float time_delta);

  void on_end_system_frame();

  void update_hierarchy(Node* const* nodes, size_t num_nodes);

  void update_child_hierarchy(
      uint32_t parent_hierachy_depth,
      bool parent_destroyed,
      std::vector<Node*>& nodes,
      size_t offset
  );

  void update_matrices(Node* const* nodes, size_t num_nodes);

  void update_child_matrices(
      const Mat4& parent_matrix,
      std::vector<Node*>& nodes,
      size_t offset,
      std::vector<ENodeTransformChanged>& out_events
  );

  TypeDB* _type_db;
  float _current_time;
  uint64_t _frame_id = 0;
  SceneData _scene_data;
  EventChannel _debug_draw_line_channel;
};
}  // namespace sge
