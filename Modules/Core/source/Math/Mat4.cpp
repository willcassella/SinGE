// Mat4.cpp

#include "../../include/Core/Math/Mat4.h"

SGE_REFLECT_TYPE(sge::Mat4)
.implements<sge::IToString>()
.property("inverse", &Mat4::inverse, nullptr);

namespace sge
{
	Mat4 Mat4::inverse() const
	{
		Mat4 result;

		///////////////////
		///  COLUMN 1   ///

		/// ROW 1
		result.set(0, 0,
			_values[1][1] * _values[2][2] * _values[3][3] +
			_values[2][1] * _values[3][2] * _values[1][3] +
			_values[3][1] * _values[1][2] * _values[2][3] -
			_values[1][1] * _values[3][2] * _values[2][3] -
			_values[2][1] * _values[1][2] * _values[3][3] -
			_values[3][1] * _values[2][2] * _values[1][3]);

		/// ROW 2
		result.set(0, 1,
			_values[0][1] * _values[3][2] * _values[2][3] +
			_values[2][1] * _values[0][2] * _values[3][3] +
			_values[3][1] * _values[2][2] * _values[0][3] -
			_values[0][1] * _values[2][2] * _values[3][3] -
			_values[2][1] * _values[3][2] * _values[0][3] -
			_values[3][1] * _values[0][2] * _values[2][3]);

		/// ROW 3
		result.set(0, 2,
			_values[0][1] * _values[1][2] * _values[3][3] +
			_values[1][1] * _values[3][2] * _values[0][3] +
			_values[3][1] * _values[0][2] * _values[1][3] -
			_values[0][1] * _values[3][2] * _values[1][3] -
			_values[1][1] * _values[0][2] * _values[3][3] -
			_values[3][1] * _values[1][2] * _values[0][3]);

		/// ROW 4
		result.set(0, 3,
			_values[0][1] * _values[2][2] * _values[1][3] +
			_values[1][1] * _values[0][2] * _values[2][3] +
			_values[2][1] * _values[1][2] * _values[0][3] -
			_values[0][1] * _values[1][2] * _values[2][3] -
			_values[1][1] * _values[2][2] * _values[0][3] -
			_values[2][1] * _values[0][2] * _values[1][3]);

		////////////////////
		///   COLUMN 2   ///

		/// ROW 1
		result.set(1, 0,
			_values[1][0] * _values[3][2] * _values[2][3] +
			_values[2][0] * _values[1][2] * _values[3][3] +
			_values[3][0] * _values[2][2] * _values[1][3] -
			_values[1][0] * _values[2][2] * _values[3][3] -
			_values[2][0] * _values[3][2] * _values[1][3] -
			_values[3][0] * _values[1][2] * _values[2][3]);

		/// ROW 2
		result.set(1, 1,
			_values[0][0] * _values[2][2] * _values[3][3] +
			_values[2][0] * _values[3][2] * _values[0][3] +
			_values[3][0] * _values[0][2] * _values[2][3] -
			_values[0][0] * _values[3][2] * _values[2][3] -
			_values[2][0] * _values[0][2] * _values[3][3] -
			_values[3][0] * _values[2][2] * _values[0][3]);

		/// ROW 3
		result.set(1, 2,
			_values[0][0] * _values[3][2] * _values[1][3] +
			_values[1][0] * _values[0][2] * _values[3][3] +
			_values[3][0] * _values[1][2] * _values[0][3] -
			_values[0][0] * _values[1][2] * _values[3][3] -
			_values[1][0] * _values[3][2] * _values[0][3] -
			_values[3][0] * _values[0][2] * _values[1][3]);

		/// ROW 4
		result.set(1, 3,
			_values[0][0] * _values[1][2] * _values[2][3] +
			_values[1][0] * _values[2][2] * _values[0][3] +
			_values[2][0] * _values[0][2] * _values[1][3] -
			_values[0][0] * _values[2][2] * _values[1][3] -
			_values[1][0] * _values[0][2] * _values[2][3] -
			_values[2][0] * _values[1][2] * _values[0][3]);

		////////////////////
		///   COLUMN 3   ///

		/// ROW 1
		result.set(2, 0,
			_values[1][0] * _values[2][1] * _values[3][3] +
			_values[2][0] * _values[3][1] * _values[1][3] +
			_values[3][0] * _values[1][1] * _values[2][3] -
			_values[1][0] * _values[3][1] * _values[2][3] -
			_values[2][0] * _values[1][1] * _values[3][3] -
			_values[3][0] * _values[2][1] * _values[1][3]);

		/// ROW 2
		result.set(2, 1,
			_values[0][0] * _values[3][1] * _values[2][3] +
			_values[2][0] * _values[0][1] * _values[3][3] +
			_values[3][0] * _values[2][1] * _values[0][3] -
			_values[0][0] * _values[2][1] * _values[3][3] -
			_values[2][0] * _values[3][1] * _values[0][3] -
			_values[3][0] * _values[0][1] * _values[2][3]);

		/// ROW 3
		result.set(2, 2,
			_values[0][0] * _values[1][1] * _values[3][3] +
			_values[1][0] * _values[3][1] * _values[0][3] +
			_values[3][0] * _values[0][1] * _values[1][3] -
			_values[0][0] * _values[3][1] * _values[1][3] -
			_values[1][0] * _values[0][1] * _values[3][3] -
			_values[3][0] * _values[1][1] * _values[0][3]);

		/// ROW 4
		result.set(2, 3,
			_values[0][0] * _values[2][1] * _values[1][3] +
			_values[1][0] * _values[0][1] * _values[2][3] +
			_values[2][0] * _values[1][1] * _values[0][3] -
			_values[0][0] * _values[1][1] * _values[2][3] -
			_values[1][0] * _values[2][1] * _values[0][3] -
			_values[2][0] * _values[0][1] * _values[1][3]);

		////////////////////
		///   COLUMN 4   ///

		/// ROW 1
		result.set(3, 0,
			_values[1][0] * _values[3][1] * _values[2][2] +
			_values[2][0] * _values[1][1] * _values[3][2] +
			_values[3][0] * _values[2][1] * _values[1][2] -
			_values[1][0] * _values[2][1] * _values[3][2] -
			_values[2][0] * _values[3][1] * _values[1][2] -
			_values[3][0] * _values[1][1] * _values[2][2]);

		/// ROW 2
		result.set(3, 1,
			_values[0][0] * _values[2][1] * _values[3][2] +
			_values[2][0] * _values[3][1] * _values[0][2] +
			_values[3][0] * _values[0][1] * _values[2][2] -
			_values[0][0] * _values[3][1] * _values[2][2] -
			_values[2][0] * _values[0][1] * _values[3][2] -
			_values[3][0] * _values[2][1] * _values[0][2]);

		/// ROW 3
		result.set(3, 2,
			_values[0][0] * _values[3][1] * _values[1][2] +
			_values[1][0] * _values[0][1] * _values[3][2] +
			_values[3][0] * _values[1][1] * _values[0][2] -
			_values[0][0] * _values[1][1] * _values[3][2] -
			_values[1][0] * _values[3][1] * _values[0][2] -
			_values[3][0] * _values[0][1] * _values[1][2]);

		/// ROW 4
		result.set(3, 3,
			_values[0][0] * _values[1][1] * _values[2][2] +
			_values[1][0] * _values[2][1] * _values[0][2] +
			_values[2][0] * _values[0][1] * _values[1][2] -
			_values[0][0] * _values[2][1] * _values[1][2] -
			_values[1][0] * _values[0][1] * _values[2][2] -
			_values[2][0] * _values[1][1] * _values[0][2]);

		return result;
	}
}
