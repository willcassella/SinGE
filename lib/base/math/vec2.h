#pragma once

#include <math.h>
#include <string>

#include "lib/base/interfaces/from_archive.h"
#include "lib/base/interfaces/to_archive.h"
#include "lib/base/reflection/reflection.h"
#include "lib/base/util/string_utils.h"

namespace sge {
struct SGE_BASE_EXPORT Vec2 {
  SGE_REFLECTED_TYPE;

  constexpr Vec2() : _values{0, 0} {}
  constexpr Vec2(float x, float y) : _values{x, y} {}

  constexpr float x() const { return _values[0]; }
  void x(float x) { _values[0] = x; }
  constexpr float y() const { return _values[1]; }
  void y(float y) { _values[1] = y; }

  const float* vec() const { return _values; }

  float* vec() { return _values; }

  /** Formats this Vec2 as a String */
  std::string to_string() const { return format("<@, @>", x(), y()); }

  /* Serializes the state of this Vec2 to an archive. */
  void to_archive(ArchiveWriter& writer) const { writer.typed_array(_values, 2); }

  /* Deserializes the state of this Vec2 from an archive. */
  void from_archive(ArchiveReader& reader) { reader.typed_array(_values, 2); }

  /** Returns the length of this vector */
  float length() const { return sqrtf(x() * x() + y() * y()); }

  /** Returns the normalized version of this vector */
  Vec2 normalized() const {
    auto len = length();

    if (len != 0) {
      return Vec2{x() / len, y() / len};
    } else {
      return Vec2::zero();
    }
  }

  /** Returns the dot product of two vectors */
  static float dot(const Vec2& a, const Vec2& b) { return a.x() * b.x() + a.y() * b.y(); }

  /** Returns the angle between two vectors */
  static float angle(const Vec2& a, const Vec2& b) {
    return acosf(Vec2::dot(a.normalized(), b.normalized()));
  }

  static Vec2 zero() { return {0, 0}; }
  static Vec2 up() { return {0, 1}; }
  static Vec2 right() { return {1, 0}; }

  friend Vec2 operator+(const Vec2& lhs, const Vec2& rhs) {
    return Vec2{lhs.x() + rhs.x(), lhs.y() + rhs.y()};
  }
  friend Vec2 operator+(const Vec2& lhs, float rhs) { return Vec2{lhs.x() + rhs, lhs.y() + rhs}; }
  friend Vec2 operator+(float lhs, const Vec2& rhs) { return Vec2{lhs + rhs.x(), lhs + rhs.y()}; }
  friend Vec2& operator+=(Vec2& lhs, const Vec2& rhs) {
    lhs = lhs + rhs;
    return lhs;
  }
  friend Vec2& operator+=(Vec2& lhs, float rhs) {
    lhs = lhs + rhs;
    return lhs;
  }
  friend Vec2 operator-(const Vec2& lhs, const Vec2& rhs) {
    return Vec2{lhs.x() - rhs.y(), lhs.y() - rhs.y()};
  }
  friend Vec2 operator-(const Vec2& lhs, float rhs) { return Vec2{lhs.x() - rhs, lhs.y() - rhs}; }
  friend Vec2 operator-(float lhs, const Vec2& rhs) { return Vec2{lhs - rhs.x(), lhs - rhs.y()}; }
  friend Vec2& operator-=(Vec2& lhs, const Vec2& rhs) {
    lhs = lhs - rhs;
    return lhs;
  }
  friend Vec2& operator-=(Vec2& lhs, float rhs) {
    lhs = lhs - rhs;
    return lhs;
  }
  friend Vec2 operator*(const Vec2& lhs, const Vec2& rhs) {
    return Vec2{lhs.x() * rhs.x(), lhs.y() * rhs.y()};
  }
  friend Vec2 operator*(const Vec2& lhs, float rhs) { return Vec2{lhs.x() * rhs, lhs.y() * rhs}; }
  friend Vec2 operator*(float lhs, const Vec2& rhs) { return Vec2{lhs * rhs.x(), lhs * rhs.y()}; }
  friend Vec2& operator*=(Vec2& lhs, const Vec2& rhs) {
    lhs = lhs * rhs;
    return lhs;
  }
  friend Vec2& operator*=(Vec2& lhs, float rhs) {
    lhs = lhs * rhs;
    return lhs;
  }
  friend Vec2 operator/(const Vec2& lhs, const Vec2& rhs) {
    return Vec2{lhs.x() / rhs.x(), lhs.y() / rhs.y()};
  }
  friend Vec2 operator/(const Vec2& lhs, float rhs) { return Vec2{lhs.x() / rhs, lhs.y() / rhs}; }
  friend Vec2 operator/(float lhs, const Vec2& rhs) { return Vec2{lhs / rhs.x(), lhs / rhs.y()}; }
  friend Vec2& operator/=(Vec2& lhs, const Vec2& rhs) {
    lhs = lhs / rhs;
    return lhs;
  }
  friend Vec2& operator/=(Vec2& lhs, float rhs) {
    lhs = lhs / rhs;
    return lhs;
  }
  friend bool operator==(const Vec2& lhs, const Vec2& rhs) {
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
  }
  friend bool operator!=(const Vec2& lhs, const Vec2& rhs) {
    return lhs.x() != rhs.x() || lhs.y() != rhs.y();
  }

 private:
  float _values[2];
};
}  // namespace sge
