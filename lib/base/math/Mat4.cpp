#include "lib/base/math/mat4.h"
#include "lib/base/reflection/reflection_builder.h"

SGE_REFLECT_TYPE(sge::Mat4)
    .implements<IToString>()
    .implements<IToArchive>()
    .implements<IFromArchive>()
    .property("inverse", &Mat4::inverse, nullptr);

namespace sge {
Mat4 Mat4::inverse() const {
  Mat4 result;

  /* Column 1 */
  /* Row 1 */
  result.set(
      0,
      0,
      _values[1][1] * _values[2][2] * _values[3][3] + _values[2][1] * _values[3][2] * _values[1][3] +
          _values[3][1] * _values[1][2] * _values[2][3] - _values[1][1] * _values[3][2] * _values[2][3] -
          _values[2][1] * _values[1][2] * _values[3][3] - _values[3][1] * _values[2][2] * _values[1][3]
  );

  /* Row 2 */
  result.set(
      0,
      1,
      _values[0][1] * _values[3][2] * _values[2][3] + _values[2][1] * _values[0][2] * _values[3][3] +
          _values[3][1] * _values[2][2] * _values[0][3] - _values[0][1] * _values[2][2] * _values[3][3] -
          _values[2][1] * _values[3][2] * _values[0][3] - _values[3][1] * _values[0][2] * _values[2][3]
  );

  /* Row 3 */
  result.set(
      0,
      2,
      _values[0][1] * _values[1][2] * _values[3][3] + _values[1][1] * _values[3][2] * _values[0][3] +
          _values[3][1] * _values[0][2] * _values[1][3] - _values[0][1] * _values[3][2] * _values[1][3] -
          _values[1][1] * _values[0][2] * _values[3][3] - _values[3][1] * _values[1][2] * _values[0][3]
  );

  /* Row 4 */
  result.set(
      0,
      3,
      _values[0][1] * _values[2][2] * _values[1][3] + _values[1][1] * _values[0][2] * _values[2][3] +
          _values[2][1] * _values[1][2] * _values[0][3] - _values[0][1] * _values[1][2] * _values[2][3] -
          _values[1][1] * _values[2][2] * _values[0][3] - _values[2][1] * _values[0][2] * _values[1][3]
  );

  /* Column 2 */
  /* Row 1 */
  result.set(
      1,
      0,
      _values[1][0] * _values[3][2] * _values[2][3] + _values[2][0] * _values[1][2] * _values[3][3] +
          _values[3][0] * _values[2][2] * _values[1][3] - _values[1][0] * _values[2][2] * _values[3][3] -
          _values[2][0] * _values[3][2] * _values[1][3] - _values[3][0] * _values[1][2] * _values[2][3]
  );

  /* Row 2 */
  result.set(
      1,
      1,
      _values[0][0] * _values[2][2] * _values[3][3] + _values[2][0] * _values[3][2] * _values[0][3] +
          _values[3][0] * _values[0][2] * _values[2][3] - _values[0][0] * _values[3][2] * _values[2][3] -
          _values[2][0] * _values[0][2] * _values[3][3] - _values[3][0] * _values[2][2] * _values[0][3]
  );

  /* Row 3 */
  result.set(
      1,
      2,
      _values[0][0] * _values[3][2] * _values[1][3] + _values[1][0] * _values[0][2] * _values[3][3] +
          _values[3][0] * _values[1][2] * _values[0][3] - _values[0][0] * _values[1][2] * _values[3][3] -
          _values[1][0] * _values[3][2] * _values[0][3] - _values[3][0] * _values[0][2] * _values[1][3]
  );

  /* Row 4 */
  result.set(
      1,
      3,
      _values[0][0] * _values[1][2] * _values[2][3] + _values[1][0] * _values[2][2] * _values[0][3] +
          _values[2][0] * _values[0][2] * _values[1][3] - _values[0][0] * _values[2][2] * _values[1][3] -
          _values[1][0] * _values[0][2] * _values[2][3] - _values[2][0] * _values[1][2] * _values[0][3]
  );

  /* Column 3 */
  /* Row 1 */
  result.set(
      2,
      0,
      _values[1][0] * _values[2][1] * _values[3][3] + _values[2][0] * _values[3][1] * _values[1][3] +
          _values[3][0] * _values[1][1] * _values[2][3] - _values[1][0] * _values[3][1] * _values[2][3] -
          _values[2][0] * _values[1][1] * _values[3][3] - _values[3][0] * _values[2][1] * _values[1][3]
  );

  /* Row 2 */
  result.set(
      2,
      1,
      _values[0][0] * _values[3][1] * _values[2][3] + _values[2][0] * _values[0][1] * _values[3][3] +
          _values[3][0] * _values[2][1] * _values[0][3] - _values[0][0] * _values[2][1] * _values[3][3] -
          _values[2][0] * _values[3][1] * _values[0][3] - _values[3][0] * _values[0][1] * _values[2][3]
  );

  /* Row 3 */
  result.set(
      2,
      2,
      _values[0][0] * _values[1][1] * _values[3][3] + _values[1][0] * _values[3][1] * _values[0][3] +
          _values[3][0] * _values[0][1] * _values[1][3] - _values[0][0] * _values[3][1] * _values[1][3] -
          _values[1][0] * _values[0][1] * _values[3][3] - _values[3][0] * _values[1][1] * _values[0][3]
  );

  /* Row 4 */
  result.set(
      2,
      3,
      _values[0][0] * _values[2][1] * _values[1][3] + _values[1][0] * _values[0][1] * _values[2][3] +
          _values[2][0] * _values[1][1] * _values[0][3] - _values[0][0] * _values[1][1] * _values[2][3] -
          _values[1][0] * _values[2][1] * _values[0][3] - _values[2][0] * _values[0][1] * _values[1][3]
  );

  /* Column 4 */
  /* Row 1 */
  result.set(
      3,
      0,
      _values[1][0] * _values[3][1] * _values[2][2] + _values[2][0] * _values[1][1] * _values[3][2] +
          _values[3][0] * _values[2][1] * _values[1][2] - _values[1][0] * _values[2][1] * _values[3][2] -
          _values[2][0] * _values[3][1] * _values[1][2] - _values[3][0] * _values[1][1] * _values[2][2]
  );

  /* Row 2 */
  result.set(
      3,
      1,
      _values[0][0] * _values[2][1] * _values[3][2] + _values[2][0] * _values[3][1] * _values[0][2] +
          _values[3][0] * _values[0][1] * _values[2][2] - _values[0][0] * _values[3][1] * _values[2][2] -
          _values[2][0] * _values[0][1] * _values[3][2] - _values[3][0] * _values[2][1] * _values[0][2]
  );

  /* Row 3 */
  result.set(
      3,
      2,
      _values[0][0] * _values[3][1] * _values[1][2] + _values[1][0] * _values[0][1] * _values[3][2] +
          _values[3][0] * _values[1][1] * _values[0][2] - _values[0][0] * _values[1][1] * _values[3][2] -
          _values[1][0] * _values[3][1] * _values[0][2] - _values[3][0] * _values[0][1] * _values[1][2]
  );

  /* Row 4 */
  result.set(
      3,
      3,
      _values[0][0] * _values[1][1] * _values[2][2] + _values[1][0] * _values[2][1] * _values[0][2] +
          _values[2][0] * _values[0][1] * _values[1][2] - _values[0][0] * _values[2][1] * _values[1][2] -
          _values[1][0] * _values[0][1] * _values[2][2] - _values[2][0] * _values[1][1] * _values[0][2]
  );

  return result;
}

void Mat4::transpose() {
  for (int x = 0; x < 4; ++x) {
    for (int y = 0; y < 4; ++y) {
      const float temp = _values[x][y];
      _values[x][y] = _values[y][x];
      _values[y][x] = temp;
    }
  }
}

Mat4 Mat4::transposed() const {
  Mat4 copy = *this;
  copy.transpose();
  return copy;
}
}  // namespace sge
