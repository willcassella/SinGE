// Quat.h
#pragma once

#include "Vec3.h"
#include "Angle.h"

namespace sge
{
	struct SGE_CORE_API Quat
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		constexpr Quat()
			: _x{ 0 }, _y{ 0 }, _z{ 0 }, _w{ 1 }
		{
		}

		Quat(Scalar x, Scalar y, Scalar z, Scalar w)
			: _x{ x }, _y{ y }, _z{ z }, _w{ w }
		{
		}

		/** Constructs a new Quaternion
		* 'axis' - The axis about which this rotation represents
		* 'angle' - The amount to rotate about 'axis' */
		Quat(const Vec3& axis, Angle angle)
		{
			// Make sure the axis vector is normalized
			const auto normAxis = axis.normalized();
			const auto sinHalfAngle = std::sin(angle * Scalar{ 0.5 });

			this->_x = normAxis.x() * sinHalfAngle;
			this->_y = normAxis.y() * sinHalfAngle;
			this->_z = normAxis.z() * sinHalfAngle;
			this->_w = std::cos(angle * Scalar { 0.5 });
		}

		////////////////
		///   Data   ///
	private:

		Scalar _x;
		Scalar _y;
		Scalar _z;
		Scalar _w;

		///////////////////
		///   Methods   ///
	public:

		Scalar x() const
		{
			return _x;
		}
		void x(Scalar x)
		{
			_x = x;
		}
		Scalar y() const
		{
			return _y;
		}
		void y(Scalar y)
		{
			_y = y;
		}
		Scalar z() const
		{
			return _z;
		}
		void z(Scalar z)
		{
			_z = z;
		}
		Scalar w() const
		{
			return _w;
		}
		void w(Scalar w)
		{
			_w = w;
		}

		/** Formats this Quat as a String */
		std::string to_string() const
		{
			return format("<@, @, @, @>", x(), y(), z(), w());
		}

		/** Rotate this quaternion around an axis by a certain angle */
		void rotate_by_axis_angle(const Vec3& axis, Angle angle, bool local)
		{
			// Construct a quaternion from the axis and angle
			Quat rotation{ axis, angle };

			if (local)
			{
				// Perform a local rotation
				*this = rotation * *this;
			}
			else
			{
				// Perform a global rotation
				*this = *this * rotation;
			}
		}

		/////////////////////
		///   Operators   ///
	public:

		friend Quat operator*(const Quat& lhs, const Quat& rhs)
		{
			Quat total;
			total._w = lhs.w() * rhs.w() - lhs.x() * rhs.x() - lhs.y() * rhs.y() - lhs.z() * rhs.z();
			total._x = lhs.w() * rhs.x() + lhs.x() * rhs.w() + lhs.y() * rhs.z() - lhs.z() * rhs.y();
			total._y = lhs.w() * rhs.y() - lhs.x() * rhs.z() + lhs.y() * rhs.w() + lhs.z() * rhs.x();
			total._z = lhs.w() * rhs.z() + lhs.x() * rhs.y() - lhs.y() * rhs.x() + lhs.z() * rhs.w();

			return total;
		}
		friend Quat& operator*=(Quat& lhs, const Quat& rhs)
		{
			lhs = lhs * rhs;
			return lhs;
		}
		friend bool operator==(const Quat& lhs, const Quat& rhs)
		{
			return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
		}
		friend bool operator!=(const Quat& lhs, const Quat& rhs)
		{
			return lhs.x() != rhs.x() || lhs.y() != rhs.y() || lhs.z() != rhs.z();
		}
	};
}
