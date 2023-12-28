#include <stdint.h>
#include <memory>

#include "lib/base/math/ivec2.h"
#include "lib/base/reflection/reflection_builder.h"
#include "lib/engine/components/display/static_mesh.h"
#include "lib/engine/scene.h"
#include "lib/engine/util/basic_component_container.h"
#include "lib/engine/util/shared_data.h"
#include "lib/resource/resources/static_mesh.h"

SGE_REFLECT_TYPE(sge::CStaticMesh)
    .property("mesh", &CStaticMesh::mesh, &CStaticMesh::mesh)
    .property("material", &CStaticMesh::material, &CStaticMesh::material)
    .property("uv_scale", &CStaticMesh::uv_scale, &CStaticMesh::uv_scale)
    .property("lightmask_mode", &CStaticMesh::lightmask_mode, &CStaticMesh::lightmask_mode)
    .property("lightmask_group", &CStaticMesh::lightmask_group, &CStaticMesh::lightmask_group)
    .property("use_lightmap", &CStaticMesh::uses_lightmap, &CStaticMesh::set_uses_lightmap)
    .property("lightmap_width", &CStaticMesh::lightmap_width, &CStaticMesh::lightmap_width)
    .property("lightmap_height", &CStaticMesh::lightmap_height, &CStaticMesh::lightmap_height);

SGE_REFLECT_ENUM(sge::CStaticMesh::LightmaskMode)
    .value("NONE", CStaticMesh::LightmaskMode::NONE)
    .value("OCCLUDER", CStaticMesh::LightmaskMode::OCCLUDER)
    .value("RECEIVER", CStaticMesh::LightmaskMode::RECEIVER);

namespace sge {
struct CStaticMesh::SharedData : CSharedData<CStaticMesh> {};

CStaticMesh::CStaticMesh(NodeId node, SharedData& shared_data) : _node(node), _shared_data(&shared_data) {}

void CStaticMesh::register_type(Scene& scene) {
  scene.register_component_type(
      type_info, std::make_unique<BasicComponentContainer<CStaticMesh, SharedData>>()
  );
}

void CStaticMesh::to_archive(ArchiveWriter& writer) const {
  writer.object_member("mesh", _mesh);
  writer.object_member("material", _material);
  writer.object_member("lmm", (int)_lightmask_mode);
  writer.object_member("lmg", _lightmask_group);
  writer.object_member("uselm", _use_lightmap);
  writer.object_member("lms", _lightmap_size);
  writer.object_member("uvs", _uv_scale);
}

void CStaticMesh::from_archive(ArchiveReader& reader) {
  reader.object_member("mesh", _mesh);
  reader.object_member("material", _material);
  reader.object_member("lmg", _lightmask_group);
  reader.object_member("uselm", _use_lightmap);
  reader.object_member("lms", _lightmap_size);
  reader.object_member("uvs", _uv_scale);

  int lightmask_mode = (int)LightmaskMode::NONE;
  reader.object_member("lmm", lightmask_mode);
  _lightmask_mode = (LightmaskMode)lightmask_mode;
}

const std::string& CStaticMesh::mesh() const {
  return _mesh;
}

void CStaticMesh::mesh(std::string mesh) {
  if (mesh != _mesh) {
    _mesh = std::move(mesh);
    set_modified("mesh");
  }
}

const std::string& CStaticMesh::material() const {
  return _material;
}

void CStaticMesh::material(std::string material) {
  if (material != _material) {
    _material = std::move(material);
    set_modified("material");
  }
}

CStaticMesh::LightmaskMode CStaticMesh::lightmask_mode() const {
  return _lightmask_mode;
}

void CStaticMesh::lightmask_mode(LightmaskMode value) {
  if (_lightmask_mode != value) {
    _lightmask_mode = value;
    set_modified("lightmask_mode");
  }
}

uint32_t CStaticMesh::lightmask_group() const {
  return _lightmask_group;
}

void CStaticMesh::lightmask_group(uint32_t value) {
  if (_lightmask_group != value) {
    _lightmask_group = value;
    set_modified("lightmask_group");
  }
}

bool CStaticMesh::uses_lightmap() const {
  return _use_lightmap;
}

void CStaticMesh::set_uses_lightmap(bool value) {
  if (_use_lightmap != value) {
    _use_lightmap = value;
    set_modified("use_lightmap");
  }
}

int32_t CStaticMesh::lightmap_width() const {
  return _lightmap_size.x();
}

void CStaticMesh::lightmap_width(int32_t width) {
  if (lightmap_width() != width) {
    _lightmap_size.x(width);
    set_modified("lightmap_width");
  }
}

int32_t CStaticMesh::lightmap_height() const {
  return _lightmap_size.y();
}

void CStaticMesh::lightmap_height(int32_t height) {
  if (lightmap_height() != height) {
    _lightmap_size.y(height);
    set_modified("lightmap_height");
  }
}

Vec2 CStaticMesh::uv_scale() const {
  return _uv_scale;
}

void CStaticMesh::uv_scale(Vec2 value) {
  if (_uv_scale != value) {
    _uv_scale = value;
    set_modified("uv_scale");
  }
}

void CStaticMesh::set_modified(const char* property_name) {
  _shared_data->set_modified(_node, this, property_name);
}
}  // namespace sge
