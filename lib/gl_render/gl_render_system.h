#pragma once

#include <memory>

#include "lib/base/reflection/reflection.h"
#include "lib/engine/scene.h"
#include "lib/gl_render/build.h"

namespace sge {
struct SystemFrame;

namespace gl_render {
struct Config;

struct SGE_GL_RENDER_API GLRenderSystem {
  SGE_REFLECTED_TYPE;
  struct State;

  GLRenderSystem(const Config& config);
  ~GLRenderSystem();

  void pipeline_register(UpdatePipeline& pipeline);

  void initialize_subscriptions(Scene& scene);

  void set_viewport(int width, int height);

  void reset();

 private:
  void render_scene(Scene& scene, SystemFrame& frame);

  std::unique_ptr<State> _state;
  EventChannel* _new_static_mesh_channel = nullptr;
  EventChannel* _modified_static_mesh_channel = nullptr;
  EventChannel* _destroyed_static_mesh_channel = nullptr;
  EventChannel* _new_spotlight_channel = nullptr;
  EventChannel* _modified_spotlight_channel = nullptr;
  EventChannel* _destroyed_spotlight_channel = nullptr;
  EventChannel* _modified_node_transform_channel = nullptr;
  EventChannel* _debug_draw_line_channel = nullptr;
  EventChannel::SubscriberId _new_static_mesh_sid = EventChannel::INVALID_SID;
  EventChannel::SubscriberId _modified_static_mesh_sid = EventChannel::INVALID_SID;
  EventChannel::SubscriberId _destroyed_static_mesh_sid = EventChannel::INVALID_SID;
  EventChannel::SubscriberId _new_spotlight_sid = EventChannel::INVALID_SID;
  EventChannel::SubscriberId _modified_spotlight_sid = EventChannel::INVALID_SID;
  EventChannel::SubscriberId _destroyed_spotlight_sid = EventChannel::INVALID_SID;
  EventChannel::SubscriberId _modified_node_transform_sid = EventChannel::INVALID_SID;
  EventChannel::SubscriberId _debug_draw_line_sid = EventChannel::INVALID_SID;
};
}  // namespace gl_render
}  // namespace sge
