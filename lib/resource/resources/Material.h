#pragma once

#include <unordered_map>

#include "lib/base/math/vec4.h"
#include "lib/resource/misc/color.h"

namespace sge {
struct SGE_RESOURCE_API Material {
  SGE_REFLECTED_TYPE;

  struct SGE_RESOURCE_API ParamTable {
    void to_archive(ArchiveWriter& writer) const;

    void from_archive(ArchiveReader& reader);

    std::unordered_map<std::string, bool> bool_params;
    std::unordered_map<std::string, float> float_params;
    std::unordered_map<std::string, Vec2> vec2_params;
    std::unordered_map<std::string, Vec3> vec3_params;
    std::unordered_map<std::string, Vec4> vec4_params;
    std::unordered_map<std::string, std::string> texture_params;
  };

  bool from_file(const char* path);

  void to_archive(ArchiveWriter& writer) const;

  void from_archive(ArchiveReader& reader);

  const std::string& vertex_shader() const { return _vertex_shader; }

  const std::string& pixel_shader() const { return _pixel_shader; }

  color::RGBF32 base_reflectivity() const { return _base_reflectivity; }

  Vec2 base_uv_scale() const { return _base_uv_scale; }

  const ParamTable& param_table() const { return _param_table; }

  void set_param(std::string name, bool value) { _param_table.bool_params[std::move(name)] = value; }

  void set_param(std::string name, float value) { _param_table.float_params[std::move(name)] = value; }

  void set_param(std::string name, Vec2 value) { _param_table.vec2_params[std::move(name)] = value; }

  void set_param(std::string name, Vec3 value) { _param_table.vec3_params[std::move(name)] = value; }

  void set_param(std::string name, Vec4 value) { _param_table.vec4_params[std::move(name)] = value; }

  void set_param(std::string name, std::string value) {
    _param_table.texture_params[std::move(name)] = std::move(value);
  }

 private:
  std::string _vertex_shader;
  std::string _pixel_shader;
  color::RGBF32 _base_reflectivity = {0.5f, 0.5f, 0.5f};
  Vec2 _base_uv_scale = {1.f, 1.f};
  ParamTable _param_table;
};
}  // namespace sge
