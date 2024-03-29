#include "lib/engine/components/physics/sphere_collider.h"
#include "lib/base/reflection/reflection_builder.h"
#include "lib/engine/scene.h"
#include "lib/engine/util/basic_component_container.h"
#include "lib/engine/util/shared_data.h"

SGE_REFLECT_TYPE(sge::CSphereCollider)
    .implements<IToArchive>()
    .implements<IFromArchive>()
    .property("radius", &CSphereCollider::radius, &CSphereCollider::radius);

namespace sge {
struct CSphereCollider::SharedData : CSharedData<CSphereCollider> {};

CSphereCollider::CSphereCollider(NodeId node, SharedData& shared_data)
    : _node(node), _shared_data(&shared_data) {}

void CSphereCollider::register_type(Scene& scene) {
  scene.register_component_type(
      type_info, std::make_unique<BasicComponentContainer<CSphereCollider, SharedData>>()
  );
}

void CSphereCollider::to_archive(ArchiveWriter& writer) const {
  writer.as_object();
  writer.object_member("r", _radius);
}

void CSphereCollider::from_archive(ArchiveReader& reader) {
  reader.object_member("r", _radius);
}

NodeId CSphereCollider::node() const {
  return _node;
}

float CSphereCollider::radius() const {
  return _radius;
}

void CSphereCollider::radius(float value) {
  if (_radius != value) {
    _radius = value;
    _shared_data->set_modified(_node, this, "radius");
  }
}
}  // namespace sge
