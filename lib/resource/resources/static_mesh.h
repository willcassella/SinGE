#pragma once

#include <stdint.h>
#include <vector>

#include "lib/base/containers/fixed_string.h"
#include "lib/base/math/ivec2.h"
#include "lib/base/math/ivec3.h"
#include "lib/resource/resources/material.h"

namespace sge {
struct SGE_RESOURCE_API StaticMesh {
  SGE_REFLECTED_TYPE;

  struct SGE_RESOURCE_API Material {
    void to_archive(ArchiveWriter& writer) const;

    void from_archive(ArchiveReader& reader);

    const std::string& path() const;

    uint32_t start_elem_index() const;

    uint32_t num_elem_indices() const;

   private:
    std::string _path;
    uint32_t _start_elem_index = 0;
    uint32_t _num_elem_indices = 0;
  };

  StaticMesh();

  void to_archive(ArchiveWriter& writer) const;

  void from_archive(ArchiveReader& reader);

  bool from_file(const char* path);

  size_t num_verts() const;

  const Vec3* vertex_positions() const;

  const HalfVec3* vertex_normals() const;

  const HalfVec3* vertex_tangents() const;

  const int8_t* bitangent_signs() const;

  const UHalfVec2* material_uv() const;

  const UHalfVec2* lightmap_uv() const;

  size_t num_triangles() const;

  size_t num_triangle_elements() const;

  const uint32_t* triangle_elements() const;

  size_t num_materials() const;

  const Material* materials() const;

 private:
  std::vector<Vec3> _vertex_positions;
  std::vector<HalfVec3> _vertex_normals;
  std::vector<HalfVec3> _vertex_tangents;
  std::vector<int8_t> _bitangent_signs;
  std::vector<UHalfVec2> _material_uv;
  std::vector<UHalfVec2> _lightmap_uv;
  std::vector<uint32_t> _triangle_elements;
  std::vector<Material> _materials;
};
}  // namespace sge
