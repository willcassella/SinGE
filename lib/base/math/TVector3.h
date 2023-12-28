#pragma once

namespace sge {
template <typename T>
struct TVector3 {
  TVector3() : value{0, 0, 0} {}
  TVector3(T x, T y, T z) : value{x, y, z} {}

  T& x() { return value[0]; }
  const T& x() const { return value[0]; }
  void x(T x) { value[0] = x; }
  T& y() { return value[1]; }
  const T& y() const { return value[1]; }
  void y(T y) { value[1] = y; }
  T& z() { return value[2]; }
  const T& z() const { return value[2]; }
  void z(T z) { value[2] = z; }

  T value[3];
};
}  // namespace sge
