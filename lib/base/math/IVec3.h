#pragma once

#include <stdint.h>

#include "lib/base/interfaces/from_archive.h"
#include "lib/base/interfaces/to_archive.h"
#include "lib/base/math/conversions.h"
#include "lib/base/reflection/reflection_builder.h"

namespace sge {
template <typename T>
struct IVec3 {
  SGE_REFLECTED_TYPE;

  IVec3() : _values{0, 0, 0} {}

  IVec3(T x, T y, T z) : _values{x, y, z} {}

  static IVec3 zero() { return IVec3{0, 0, 0}; }

  void to_archive(ArchiveWriter& writer) const { writer.typed_array(_values, 3); }

  void from_archive(ArchiveReader& reader) { reader.typed_array(_values, 3); }

  const T* vec() const { return _values; }

  T* vec() { return _values; }

  T x() const { return _values[0]; }

  void x(T value) { _values[0] = value; }

  T y() const { return _values[1]; }

  void y(T value) { _values[1] = value; }

  T z() const { return _values[2]; }

  void z(T value) { _values[2] = value; }

  float norm_f32_x() const { return to_norm_f32(_values[0]); }

  void norm_f32_x(float value) { _values[0] = from_norm_f32<T>(value); }

  float norm_f32_y() const { return to_norm_f32(_values[1]); }

  void norm_f32_y(float value) { _values[1] = from_norm_f32<T>(value); }

  float norm_f32_z() const { return to_norm_f32(_values[2]); }

  void norm_f32_z(float value) { _values[2] = from_norm_f32<T>(value); }

  friend bool operator==(const IVec3& lhs, const IVec3& rhs) {
    return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
  }
  friend bool operator!=(const IVec3& lhs, const IVec3& rhs) { return !(lhs == rhs); }

 private:
  T _values[3];
};

template <typename T>
SGE_REFLECT_TYPE_TEMPLATE(IVec3, T).template implements<IToArchive>().template implements<IFromArchive>();

using HalfVec3 = IVec3<int16_t>;
using UHalfVec3 = IVec3<uint16_t>;
}  // namespace sge
