#pragma once

#include <stdint.h>
#include <algorithm>
#include <map>
#include <set>
#include <vector>

#include "lib/base/interfaces/from_string.h"
#include "lib/base/memory/buffers/multi_stack_buffer.h"
#include "lib/engine/component.h"

namespace sge {
template <class ComponentT, typename SharedDataT>
class BasicComponentContainer final : public ComponentContainer {
 public:
  BasicComponentContainer()
      : _new_instance_channel(sizeof(ENewComponent), 8),
        _destroyed_instance_channel(sizeof(EDestroyedComponent), 8) {}

  const TypeInfo& get_component_type() const override { return sge::get_type<ComponentT>(); }

  void reset() override {
    _shared_data.reset();
    _new_instance_channel.clear();
    _destroyed_instance_channel.clear();
    _destroyed_instances.clear();
    _instance_map.clear();
    _instance_nodes.clear();
    _instance_buffer.clear();
    _free_spots.clear();
  }

  void to_archive(ArchiveWriter& writer) const override {
    char id_str[20];

    for (auto instance : _instance_map) {
      instance.first.to_string(id_str, 20);
      writer.push_object_member(id_str);
      instance.second->to_archive(writer);
      writer.pop();
    }
  }

  void from_archive(ArchiveReader& reader) override {
    reset();

    std::vector<ENewComponent> new_instances;

    reader.enumerate_object_members([this, &reader, &new_instances](const char* id_str) {
      // Get the node id
      NodeId node;
      node.from_string(id_str);

      // Make sure it's valid
      if (node.is_null()) {
        return;
      }

      // Make sure it doesn't already exist in the map
      const auto node_iter = this->_instance_map.find(node);
      if (node_iter != this->_instance_map.end()) {
        return;
      }

      // Allocate space for the node
      auto* const buff = this->_instance_buffer.alloc(sizeof(ComponentT));
      auto* const instance = new (buff) ComponentT(node, _shared_data);

      // Insert it into the map
      this->_instance_map[node] = instance;
      this->_instance_nodes.push_back(node);

      // Deserialize it
      instance->from_archive(reader);

      // Create the new instance event
      ENewComponent event;
      event.node = node;
      event.instance = instance;
      new_instances.push_back(event);
    });

    // Append all new instance events
    _new_instance_channel.append(new_instances.data(), sizeof(ENewComponent), (int32_t)new_instances.size());
  }

  void on_end_system_frame() override { _shared_data.on_end_system_frame(); }

  void on_end_update_frame() override {
    // Destroy instances
    for (const auto destroyed_instance : _destroyed_instances) {
      const auto map_iter = _instance_map.find(destroyed_instance);
      const auto inst_iter = std::find(_instance_nodes.begin(), _instance_nodes.end(), destroyed_instance);

      // Call the destructor
      map_iter->second->~ComponentT();

      // Allow the system to reuse this memory location
      _free_spots.push_back(map_iter->second);

      // Remove it from the map
      _instance_map.erase(map_iter);

      // Remove it from the set of instance nodes
      _instance_nodes.erase(inst_iter);
    }
    _destroyed_instances.clear();

    // Clear events
    _shared_data.on_end_update_frame();
    _new_instance_channel.clear();
    _destroyed_instance_channel.clear();
  }

  void create_instances(const Node* const* nodes, size_t num_nodes, void** out_instances) override {
    std::vector<ENewComponent> new_instances;
    new_instances.reserve(num_nodes);

    for (size_t i = 0; i < num_nodes; ++i) {
      const auto node = nodes[i];
      if (!node) {
        out_instances[i] = nullptr;
        continue;
      }

      const auto node_id = node->get_id();

      // Make sure the instance doesn't already exist
      const auto iter = _instance_map.find(node_id);
      if (iter != _instance_map.end()) {
        out_instances[i] = nullptr;
        continue;
      }

      // Allocate memory
      void* buff;
      if (_free_spots.empty()) {
        buff = _instance_buffer.alloc(sizeof(ComponentT));
      } else {
        buff = _free_spots.back();
        _free_spots.erase(_free_spots.end() - 1);
      }

      // Construct the instance
      auto* const instance = new (buff) ComponentT(node_id, _shared_data);
      out_instances[i] = instance;

      // Add it to the map
      _instance_map[node_id] = instance;

      // Add it to the instance node list
      _instance_nodes.push_back(node_id);

      // Create the new instance event
      ENewComponent event;
      event.node = node_id;
      event.instance = instance;
      new_instances.push_back(event);

      // If the node is destroyed, add the new instance to the destroyed list
      if (node->get_mod_state() & Node::DESTROYED_APPLIED) {
        EDestroyedComponent destroyed_event;
        destroyed_event.node = node_id;
        destroyed_event.instance = instance;
        _destroyed_instance_channel.append(&destroyed_event, 1);
        _destroyed_instances.insert(node_id);
      }
    }

    // Create the new instance events
    _new_instance_channel.append(new_instances.data(), sizeof(ENewComponent), (int32_t)new_instances.size());
  }

  void remove_instances(const NodeId* nodes, size_t num_nodes) override {
    std::vector<EDestroyedComponent> destroyed_events;
    destroyed_events.reserve(num_nodes);

    // Figure out which instances of the given nodes actually have these components
    for (size_t i = 0; i < num_nodes; ++i) {
      const auto node = nodes[i];

      // See if this component actually exists, or if it's already been deleted
      const auto inst_iter = _instance_map.find(node);
      const auto dest_iter = _destroyed_instances.find(node);

      if (inst_iter == _instance_map.end() || dest_iter != _destroyed_instances.end()) {
        continue;
      }

      _destroyed_instances.insert(node);

      // Create the destroyed event
      EDestroyedComponent event;
      event.node = node;
      event.instance = inst_iter->second;
      destroyed_events.push_back(event);
    }

    // Add all events
    _destroyed_instance_channel.append(
        destroyed_events.data(), sizeof(EDestroyedComponent), (int32_t)destroyed_events.size()
    );
  }

  void get_instances(const NodeId* nodes, size_t num_instances, void** out_instances) override {
    for (size_t i = 0; i < num_instances; ++i) {
      const auto node = nodes[i];

      // Search for the id
      const auto iter = _instance_map.find(node);
      if (iter == _instance_map.end()) {
        out_instances[i] = nullptr;
        continue;
      }

      out_instances[i] = iter->second;
    }
  }

  size_t num_instance_nodes() const override { return _instance_nodes.size(); }

  size_t get_instance_nodes(
      size_t start_index,
      size_t num_instances,
      size_t* out_num_instances,
      NodeId* out_instances
  ) const override {
    if (start_index >= _instance_nodes.size()) {
      *out_num_instances = 0;
      return 0;
    }

    const auto num_copy = std::min(_instance_nodes.size() - start_index, num_instances);
    memcpy(out_instances, _instance_nodes.data() + start_index, num_copy * sizeof(NodeId));
    *out_num_instances = num_copy;
    return num_copy;
  }

  EventChannel* get_event_channel(const char* name) override {
    if (strcmp(name, "new") == 0) {
      return &_new_instance_channel;
    } else if (strcmp(name, "destroy") == 0) {
      return &_destroyed_instance_channel;
    } else {
      return _shared_data.get_event_channel(name);
    }
  }

 private:
  SharedDataT _shared_data;
  EventChannel _new_instance_channel;
  EventChannel _destroyed_instance_channel;
  std::set<NodeId> _destroyed_instances;
  std::map<NodeId, ComponentT*> _instance_map;
  std::vector<NodeId> _instance_nodes;
  MultiStackBuffer _instance_buffer;
  std::vector<void*> _free_spots;
};
}  // namespace sge
