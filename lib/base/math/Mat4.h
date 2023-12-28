#pragma once

#include <assert.h>
#include <stdint.h>

#include "lib/base/math/quat.h"

namespace sge {
struct SGE_BASE_EXPORT Mat4 {
  SGE_REFLECTED_TYPE;

  /** Constructs a new 4x4 matrix
  default - identity matrix
  values - what to set each element of the matrix to */
  Mat4(
      float aa = 1.f,
      float ba = 0.f,
      float ca = 0.f,
      float da = 0.f,
      float ab = 0.f,
      float bb = 1.f,
      float cb = 0.f,
      float db = 0.f,
      float ac = 0.f,
      float bc = 0.f,
      float cc = 1.f,
      float dc = 0.f,
      float ad = 0.f,
      float bd = 0.f,
      float cd = 0.f,
      float dd = 1.f
  ) {
    _values[0][0] = aa;
    _values[1][0] = ba;
    _values[2][0] = ca;
    _values[3][0] = da;
    _values[0][1] = ab;
    _values[1][1] = bb;
    _values[2][1] = cb;
    _values[3][1] = db;
    _values[0][2] = ac;
    _values[1][2] = bc;
    _values[2][2] = cc;
    _values[3][2] = dc;
    _values[0][3] = ad;
    _values[1][3] = bd;
    _values[2][3] = cd;
    _values[3][3] = dd;
  }

  const float* vec() const { return &_values[0][0]; }

  float* vec() { return &_values[0][0]; }

  /** Formats this Mat4 as a String */
  std::string to_string() const {
    return format(
        "[ @, @, @, @ ]\n"
        "| @, @, @, @ |\n"
        "| @, @, @, @ |\n"
        "[ @, @, @, @ ]",
        _values[0][0],
        _values[1][0],
        _values[2][0],
        _values[3][0],
        _values[0][1],
        _values[1][1],
        _values[2][1],
        _values[3][1],
        _values[0][2],
        _values[1][2],
        _values[2][2],
        _values[3][2],
        _values[0][3],
        _values[1][3],
        _values[2][3],
        _values[3][3]
    );
  }

  /* Serializes the state of this Mat4 to an archive. */
  void to_archive(ArchiveWriter& writer) const { writer.typed_array(_values[0], 16); }

  /* Deserializes the state of this Mat4 from an archive. */
  void from_archive(ArchiveReader& reader) { reader.typed_array(_values[0], 16); }

  /** Returns the inverse of this matrix */
  Mat4 inverse() const;

  /**
   * \brief Transposes this matrix;
   */
  void transpose();

  /**
   * \brief Returns the transpose of this matrix.
   */
  Mat4 transposed() const;

  /** Generates a perspective projection matrix with the given properties */
  static Mat4 perspective_projection(Angle hFOV, Angle vFOV, float zMin, float zMax) {
    const auto xMax = tanf(hFOV * 0.5f) * zMin;
    const auto xMin = -xMax;

    const auto yMax = tanf(vFOV * 0.5f) * zMin;
    const auto yMin = -yMax;

    const auto width = xMax - xMin;
    const auto height = yMax - yMin;
    const auto depth = zMax - zMin;

    return Mat4{
        2 * zMin / width,
        0,
        (xMax + xMin) / width,
        0,
        0,
        2 * zMin / height,
        (yMax + yMin) / height,
        0,
        0,
        0,
        -(zMax + zMin) / depth,
        -2 * zMax * zMin / depth,
        0,
        0,
        -1,
        0
    };
  }

  /** Generates a perspective projection matrix with the following horizontal FOV */
  static Mat4 perspective_projection_hfov(Angle hFOV, float ratio, float zMin, float zMax) {
    // Calculate vertical field of view
    const Angle vFOV = 2 * atanf(tanf(hFOV * 0.5f) * (1.f / ratio));
    return perspective_projection(hFOV, vFOV, zMin, zMax);
  }

  /** Generates a perspective projection matrix with the following vertical FOV */
  static Mat4 perspective_project_vfov(Angle vFOV, float ratio, float zMin, float zMax) {
    // Calculate horizontal field of view
    const Angle hFOV = 2 * atanf(tanf(vFOV * 0.5f) * ratio);
    return perspective_projection(hFOV, vFOV, zMin, zMax);
  }

  /** Generates an orthographic projection matrix with the given properties */
  static Mat4
  orthographic_projection(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) {
    const auto width = xMax - xMin;
    const auto height = yMax - yMin;
    const auto depth = zMax - zMin;

    return Mat4{
        2 / width,
        0,
        0,
        -(xMax + xMin) / width,
        0,
        2 / height,
        0,
        -(yMax + yMin) / height,
        0,
        0,
        -2 / depth,
        -(zMax + zMin) / depth,
        0,
        0,
        0,
        1
    };
  }

  /** Generates a transformation matrix representing a translation */
  static Mat4 translation(const Vec3& vec) {
    return Mat4{1, 0, 0, vec.x(), 0, 1, 0, vec.y(), 0, 0, 1, vec.z(), 0, 0, 0, 1};
  }

  /** Generates a transformation matrix representing a Scale */
  static Mat4 scale(const Vec3& vec) {
    return Mat4{vec.x(), 0, 0, 0, 0, vec.y(), 0, 0, 0, 0, vec.z(), 0, 0, 0, 0, 1};
  }

  /** Generates a transformation matrix representing a rotation */
  static Mat4 rotate(const Quat& rot) {
    float x, y, z, w;
    x = rot.x();
    y = rot.y();
    z = rot.z();
    w = rot.w();

    return Mat4{
        1 - 2 * y * y - 2 * z * z,
        2 * x * y - 2 * z * w,
        2 * x * z + 2 * y * w,
        0,
        2 * x * y + 2 * z * w,
        1 - 2 * x * x - 2 * z * z,
        2 * y * z - 2 * x * w,
        0,
        2 * x * z - 2 * y * w,
        2 * y * z + 2 * x * w,
        1 - 2 * x * x - 2 * y * y,
        0,
        0,
        0,
        0,
        1
    };
  }

  /** Gets the value at the specified column and row */
  float get(uint32_t column, uint32_t row) const {
    assert(column < 4 && row < 4);
    return _values[column][row];
  }

  /** Sets the value at the specified column and row */
  void set(uint32_t column, uint32_t row, float value) {
    assert(column < 4 && row < 4);
    _values[column][row] = value;
  }

  float* operator[](uint32_t index) {
    assert(index < 16);
    return _values[index];
  }
  const float* operator[](uint32_t index) const {
    assert(index < 16);
    return _values[index];
  }
  friend Mat4 operator*(const Mat4& lhs, const Mat4& rhs) {
    Mat4 total;

    // For each row
    for (uint32_t row = 0; row < 4; ++row) {
      // For each column
      for (uint32_t col = 0; col < 4; ++col) {
        float value = 0;

        // For each addition
        for (uint32_t i = 0; i < 4; ++i) {
          // add them up
          value += lhs.get(i, row) * rhs.get(col, i);
        }

        // Assign it to the new matrix
        total.set(col, row, value);
      }
    }

    // Return the product of the two matrices
    return total;
  }
  friend Mat4& operator*=(Mat4& lhs, const Mat4& rhs) {
    lhs = lhs * rhs;
    return lhs;
  }
  friend Vec3 operator*(const Mat4& lhs, const Vec3& rhs) {
    Vec3 result;
    result.x(lhs.get(0, 0) * rhs.x() + lhs.get(1, 0) * rhs.y() + lhs.get(2, 0) * rhs.z() + lhs.get(3, 0));
    result.y(lhs.get(0, 1) * rhs.x() + lhs.get(1, 1) * rhs.y() + lhs.get(2, 1) * rhs.z() + lhs.get(3, 1));
    result.z(lhs.get(0, 2) * rhs.x() + lhs.get(1, 2) * rhs.y() + lhs.get(2, 2) * rhs.z() + lhs.get(3, 2));

    return result;
  }
  friend bool operator==(const Mat4& lhs, const Mat4& rhs) {
    for (int i = 0; i < 16; ++i) {
      if (lhs[i] != rhs[i]) {
        return false;
      }
    }

    return true;
  }
  friend bool operator!=(const Mat4& lhs, const Mat4& rhs) { return !(lhs == rhs); }

 private:
  float _values[4][4];
};
}  // namespace sge
