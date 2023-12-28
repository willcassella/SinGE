#include "lib/engine/components/gameplay/input.h"
#include "lib/base/reflection/reflection_builder.h"
#include "lib/engine/scene.h"
#include "lib/engine/util/basic_component_container.h"
#include "lib/engine/util/shared_data.h"

SGE_REFLECT_TYPE(sge::CInput);

namespace sge {
struct CInput::SharedData : CSharedData<CInput> {
  SharedData() : action_event_channel(sizeof(EAction), 8), axis_event_channel(sizeof(EAxis), 8) {}

  void reset() {
    action_event_channel.clear();
    axis_event_channel.clear();
    CSharedData<CInput>::reset();
  }

  void on_end_update_frame() {
    action_event_channel.clear();
    axis_event_channel.clear();
    CSharedData<CInput>::reset();
  }

  EventChannel* get_event_channel(const char* name) {
    if (strcmp(name, "action_event") == 0) {
      return &action_event_channel;
    } else if (strcmp(name, "axis_event") == 0) {
      return &axis_event_channel;
    }

    return CSharedData<CInput>::get_event_channel(name);
  }

  EventChannel action_event_channel;
  EventChannel axis_event_channel;
};

CInput::CInput(NodeId node, SharedData& shared_data) : _node(node), _shared_data(&shared_data) {}

void CInput::register_type(Scene& scene) {
  scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CInput, SharedData>>());
}

NodeId CInput::node() const {
  return _node;
}

void CInput::to_archive(ArchiveWriter& /*writer*/) const {}

void CInput::from_archive(ArchiveReader& /*reader*/) {}

void CInput::add_action_event(EventName_t action_name) const {
  EAction event;
  event.input_node = _node;
  event.name = action_name;
  _shared_data->action_event_channel.append(&event, sizeof(EAction), 1);
}

void CInput::add_axis_event(EventName_t axis_name, float value, float min, float max) const {
  EAxis event;
  event.input_node = _node;
  event.name = axis_name;
  event.value = value;
  event.min = min;
  event.max = max;
  _shared_data->axis_event_channel.append(&event, sizeof(EAxis), 1);
}
}  // namespace sge
