#pragma once

#include "lib/base/math/vec2.h"

namespace sge {
struct SGE_BASE_EXPORT Vec3 {
  SGE_REFLECTED_TYPE;

  constexpr Vec3() : _values{0, 0, 0} {}
  constexpr Vec3(float x, float y, float z) : _values{x, y, z} {}
  constexpr Vec3(const Vec2& xy, float z) : _values{xy.x(), xy.y(), z} {}
  constexpr Vec3(float x, const Vec2& yz) : _values{x, yz.x(), yz.y()} {}

  constexpr float x() const { return _values[0]; }
  void x(float x) { _values[0] = x; }
  constexpr float y() const { return _values[1]; }
  void y(float y) { _values[1] = y; }
  constexpr float z() const { return _values[2]; }
  void z(float z) { _values[2] = z; }

  const float* vec() const { return _values; }

  float* vec() { return _values; }

  /** Formats this Vec3 as a String */
  std::string to_string() const { return format("<@, @, @>", x(), y(), z()); }

  /* Serializes the state of this Vec3 to an archive. */
  void to_archive(ArchiveWriter& writer) const { writer.typed_array(_values, 3); }

  /* Deserializes the state of this Vec3 from an archive. */
  void from_archive(ArchiveReader& reader) { reader.typed_array(_values, 3); }

  /** Returns the length of this vector */
  float length() const { return sqrtf(x() * x() + y() * y() + z() * z()); }

  /** Returns the normalized version of this vector */
  Vec3 normalized() const {
    auto len = length();

    if (len != 0) {
      return Vec3{x() / len, y() / len, z() / len};
    } else {
      return Vec3::zero();
    }
  }

  static Vec3 reflect(const Vec3& a, const Vec3& b) { return 2 * Vec3::dot(a, b) * a - b; }

  /** Returns the dot product of two vectors */
  static float dot(const Vec3& a, const Vec3& b) { return a.x() * b.x() + a.y() * b.y() + a.z() * b.z(); }

  /** Returns the cross product of two vectors */
  static Vec3 cross(const Vec3& a, const Vec3& b) {
    return Vec3{a.y() * b.z() - a.z() * b.y(), a.z() * b.x() - a.x() * b.z(), a.x() * b.y() - a.y() * b.x()};
  }

  /** Returns the angle between two vectors */
  static float angle(const Vec3& a, const Vec3& b) {
    return acosf(Vec3::dot(a.normalized(), b.normalized()));
  }

  static Vec3 zero() { return {0, 0, 0}; }
  static Vec3 up() { return {0, 1, 0}; }
  static Vec3 forward() { return {0, 0, -1}; }
  static Vec3 right() { return {1, 0, 0}; }

  friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs) {
    return Vec3{lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z()};
  }
  friend Vec3 operator+(const Vec3& lhs, float rhs) {
    return Vec3{lhs.x() + rhs, lhs.y() + rhs, lhs.z() + rhs};
  }
  friend Vec3 operator+(float lhs, const Vec3& rhs) {
    return Vec3{lhs + rhs.x(), lhs + rhs.y(), lhs + rhs.z()};
  }
  friend Vec3& operator+=(Vec3& lhs, const Vec3& rhs) {
    lhs = lhs + rhs;
    return lhs;
  }
  friend Vec3& operator+=(Vec3& lhs, float rhs) {
    lhs = lhs + rhs;
    return lhs;
  }
  friend Vec3 operator-(const Vec3& lhs, const Vec3& rhs) {
    return Vec3{lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z()};
  }
  friend Vec3 operator-(const Vec3& lhs, float rhs) {
    return Vec3{lhs.x() - rhs, lhs.y() - rhs, lhs.z() - rhs};
  }
  friend Vec3 operator-(float lhs, const Vec3& rhs) {
    return Vec3{lhs - rhs.x(), lhs - rhs.y(), lhs - rhs.z()};
  }
  friend Vec3& operator-=(Vec3& lhs, const Vec3& rhs) {
    lhs = lhs - rhs;
    return lhs;
  }
  friend Vec3& operator-=(Vec3& lhs, float rhs) {
    lhs = lhs - rhs;
    return lhs;
  }
  friend Vec3 operator*(const Vec3& lhs, const Vec3& rhs) {
    return Vec3{lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z()};
  }
  friend Vec3 operator*(const Vec3& lhs, float rhs) {
    return Vec3{lhs.x() * rhs, lhs.y() * rhs, lhs.z() * rhs};
  }
  friend Vec3 operator*(float lhs, const Vec3& rhs) {
    return Vec3{lhs * rhs.x(), lhs * rhs.y(), lhs * rhs.z()};
  }
  friend Vec3& operator*=(Vec3& lhs, const Vec3& rhs) {
    lhs = lhs * rhs;
    return lhs;
  }
  friend Vec3& operator*=(Vec3& lhs, float rhs) {
    lhs = lhs * rhs;
    return lhs;
  }
  friend Vec3 operator/(const Vec3& lhs, const Vec3& rhs) {
    return Vec3{lhs.x() / rhs.x(), lhs.y() / rhs.y(), lhs.z() / rhs.z()};
  }
  friend Vec3 operator/(const Vec3& lhs, float rhs) {
    return Vec3{lhs.x() / rhs, lhs.y() / rhs, lhs.z() / rhs};
  }
  friend Vec3 operator/(float lhs, const Vec3& rhs) {
    return Vec3{lhs / rhs.x(), lhs / rhs.y(), lhs / rhs.z()};
  }
  friend Vec3& operator/=(Vec3& lhs, const Vec3& rhs) {
    lhs = lhs / rhs;
    return lhs;
  }
  friend Vec3& operator/=(Vec3& lhs, float rhs) {
    lhs = lhs / rhs;
    return lhs;
  }
  friend bool operator==(const Vec3& lhs, const Vec3& rhs) {
    return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
  }
  friend bool operator!=(const Vec3& lhs, const Vec3& rhs) {
    return lhs.x() != rhs.x() || lhs.y() != rhs.y() || lhs.z() != rhs.z();
  }

 private:
  float _values[3];
};
}  // namespace sge
