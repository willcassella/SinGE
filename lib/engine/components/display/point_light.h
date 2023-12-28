#pragma once

#include <stdint.h>

#include "lib/engine/component.h"
#include "lib/resource/misc/color.h"

namespace sge {
struct SGE_ENGINE_API CPointLight {
  SGE_REFLECTED_TYPE;

  struct SharedData;

  explicit CPointLight(const NodeId node, SharedData& shared_data);

  static void register_type(Scene& scene);

  void to_archive(ArchiveWriter& writer) const;

  void from_archive(ArchiveReader& reader);

  uint32_t update_revision() const;

  NodeId node() const;

  float radius() const;

  void radius(float value);

  color::RGBF32 intensity() const;

  void intensity(color::RGBF32 value);

 private:
  void set_modified(const char* prop_name);

  uint32_t _update_revision = 0;
  float _radius = 1.f;
  color::RGBF32 _intensity = color::RGBF32::white();
  NodeId _node;
  SharedData* _shared_data;
};
}  // namespace sge
