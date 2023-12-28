#include <stdint.h>

#include "lib/editor_server/editor_server_system_data.h"
#include "lib/engine/update_pipeline.h"

namespace sge {
EditorServerSystem::EditorServerSystem(uint16_t port) : _serve_time_ms(8) {
  _data = std::make_unique<Data>(port);
}

EditorServerSystem::~EditorServerSystem() {}

void EditorServerSystem::register_pipeline(UpdatePipeline& pipeline) {
  pipeline.register_system_fn("editor_server_serve", this, &EditorServerSystem::serve_fn);
}

int EditorServerSystem::get_serve_time() const {
  return _serve_time_ms;
}

void EditorServerSystem::set_serve_time(int milliseconds) {
  _serve_time_ms = milliseconds;
}

void EditorServerSystem::serve_fn(Scene& scene, SystemFrame& frame) {
  // Run the service
  _data->frame = &frame;
  _data->scene = &scene;
  _data->io.poll();
}
}  // namespace sge
