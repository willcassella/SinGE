#pragma once

#include <stdint.h>
#include <vector>

#include "lib/engine/component.h"
#include "lib/engine/event_channel.h"

namespace sge {
template <class ComponentT>
struct CSharedData {
  CSharedData() : modified_instance_channel(sizeof(EModifiedComponent), 8) {}

  void reset() {
    modified_instances.clear();
    modified_instance_channel.clear();
  }

  void on_end_system_frame() {
    // Add events to the channel
    modified_instance_channel.append(
        modified_instances.data(), sizeof(EModifiedComponent), (int32_t)modified_instances.size()
    );

    modified_instances.clear();
  }

  void on_end_update_frame() { modified_instance_channel.clear(); }

  EventChannel* get_event_channel(const char* name) {
    if (strcmp(name, "prop_mod") == 0) {
      return &modified_instance_channel;
    } else {
      return nullptr;
    }
  }

  void set_modified(NodeId node, ComponentT* instance, const char* prop_name) {
    EModifiedComponent event;
    event.node = node;
    event.instance = instance;
    event.property = prop_name;

    modified_instances.push_back(event);
  }

  std::vector<EModifiedComponent> modified_instances;
  EventChannel modified_instance_channel;
};
}  // namespace sge
