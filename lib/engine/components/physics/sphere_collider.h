#pragma once

#include "lib/engine/component.h"

namespace sge {
struct SGE_ENGINE_API CSphereCollider {
  struct SharedData;
  SGE_REFLECTED_TYPE;

  CSphereCollider(NodeId node, SharedData& shared_data);

  static void register_type(Scene& scene);

  void to_archive(ArchiveWriter& writer) const;

  void from_archive(ArchiveReader& reader);

  NodeId node() const;

  float radius() const;

  void radius(float value);

 private:
  float _radius = 1.f;
  NodeId _node;
  SharedData* _shared_data;
};
}  // namespace sge
