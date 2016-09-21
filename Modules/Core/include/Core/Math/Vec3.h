// Vec3.h
#pragma once

#include "Vec2.h"

namespace sge
{
	struct SGE_CORE_API Vec3
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		constexpr Vec3()
			: _values{ 0, 0, 0 }
		{
		}
		constexpr Vec3(Scalar x, Scalar y, Scalar z)
			: _values{ x, y, z }
		{
		}
		constexpr Vec3(const Vec2& xy, Scalar z)
			: _values{ xy.x(), xy.y(), z }
		{
		}
		constexpr Vec3(Scalar x, const Vec2& yz)
			: _values{ x, yz.x(), yz.y() }
		{
		}

		///////////////////
		///   Methods   ///
	public:

		constexpr Scalar x() const
		{
			return _values[0];
		}
		void x(Scalar x)
		{
			_values[0] = x;
		}
		constexpr Scalar y() const
		{
			return _values[1];
		}
		void y(Scalar y)
		{
			_values[1] = y;
		}
		constexpr Scalar z() const
		{
			return _values[2];
		}
		void z(Scalar z)
		{
			_values[2] = z;
		}

		/** Formats this Vec3 as a String */
		std::string to_string() const
		{
			return format("<@, @, @>", x(), y(), z());
		}

		/** Returns the length of this vector */
		Scalar length() const
		{
			return std::sqrt(x()*x() + y()*y() + z()*z());
		}

		/** Returns the normalized version of this vector */
		Vec3 normalized() const
		{
			auto len = length();

			if (len != 0)
			{
				return Vec3{ x()/len, y()/len, z()/len };
			}
			else
			{
				return Vec3::zero;
			}
		}

		static Vec3 reflect(const Vec3& a, const Vec3& b)
		{
			return 2 * Vec3::dot(a, b) * a - b;
		}

		/** Returns the dot product of two vectors */
		static Scalar dot(const Vec3& a, const Vec3& b)
		{
			return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
		}

		/** Returns the cross product of two vectors */
		static Vec3 cross(const Vec3& a, const Vec3& b)
		{
			return Vec3{
				a.y() * b.z() - a.z() * b.y(),
				a.z() * b.x() - a.x() * b.z(),
				a.x() * b.y() - a.y() * b.x()
			};
		}

		/** Returns the angle between two vectors */
		static Scalar angle(const Vec3& a, const Vec3& b)
		{
			return std::acos(Vec3::dot(a.normalized(), b.normalized()));
		}

		////////////////////////////
		///   Static Instances   ///
	public:

		static const Vec3 zero;
		static const Vec3 up;
		static const Vec3 forward;
		static const Vec3 right;

		/////////////////////
		///   Operators   ///
	public:

		friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
		{
			return Vec3{ lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z() };
		}
		friend Vec3 operator+(const Vec3& lhs, Scalar rhs)
		{
			return Vec3{ lhs.x() + rhs, lhs.y() + rhs, lhs.z() + rhs };
		}
		friend Vec3 operator+(Scalar lhs, const Vec3& rhs)
		{
			return Vec3{ lhs + rhs.x(), lhs + rhs.y(), lhs + rhs.z() };
		}
		friend Vec3& operator+=(Vec3& lhs, const Vec3& rhs)
		{
			lhs = lhs + rhs;
			return lhs;
		}
		friend Vec3& operator+=(Vec3& lhs, Scalar rhs)
		{
			lhs = lhs + rhs;
			return lhs;
		}
		friend Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
		{
			return Vec3{ lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z() };
		}
		friend Vec3 operator-(const Vec3& lhs, Scalar rhs)
		{
			return Vec3{ lhs.x() - rhs, lhs.y() - rhs, lhs.z() - rhs };
		}
		friend Vec3 operator-(Scalar lhs, const Vec3& rhs)
		{
			return Vec3{ lhs - rhs.x(), lhs - rhs.y(), lhs - rhs.z() };
		}
		friend Vec3& operator-=(Vec3& lhs, const Vec3& rhs)
		{
			lhs = lhs - rhs;
			return lhs;
		}
		friend Vec3& operator-=(Vec3& lhs, Scalar rhs)
		{
			lhs = lhs - rhs;
			return lhs;
		}
		friend Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
		{
			return Vec3{ lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z() };
		}
		friend Vec3 operator*(const Vec3& lhs, Scalar rhs)
		{
			return Vec3{ lhs.x() * rhs, lhs.y() * rhs, lhs.z() * rhs };
		}
		friend Vec3 operator*(Scalar lhs, const Vec3& rhs)
		{
			return Vec3{ lhs * rhs.x(), lhs * rhs.y(), lhs * rhs.z() };
		}
		friend Vec3& operator*=(Vec3& lhs, const Vec3& rhs)
		{
			lhs = lhs * rhs;
			return lhs;
		}
		friend Vec3& operator*=(Vec3& lhs, Scalar rhs)
		{
			lhs = lhs * rhs;
			return lhs;
		}
		friend Vec3 operator/(const Vec3& lhs, const Vec3& rhs)
		{
			return Vec3{ lhs.x() / rhs.x(), lhs.y() / rhs.y(), lhs.z() / rhs.z() };
		}
		friend Vec3 operator/(const Vec3& lhs, Scalar rhs)
		{
			return Vec3{ lhs.x() / rhs, lhs.y() / rhs, lhs.z() / rhs };
		}
		friend Vec3 operator/(Scalar lhs, const Vec3& rhs)
		{
			return Vec3{ lhs / rhs.x(), lhs / rhs.y(), lhs / rhs.z() };
		}
		friend Vec3& operator/=(Vec3& lhs, const Vec3& rhs)
		{
			lhs = lhs / rhs;
			return lhs;
		}
		friend Vec3& operator/=(Vec3& lhs, Scalar rhs)
		{
			lhs = lhs / rhs;
			return lhs;
		}
		friend bool operator==(const Vec3& lhs, const Vec3& rhs)
		{
			return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
		}
		friend bool operator!=(const Vec3& lhs, const Vec3& rhs)
		{
			return lhs.x() != rhs.x() || lhs.y() != rhs.y() || lhs.z() != rhs.z();
		}

		//////////////////
		///   Fields   ///
	private:

		Scalar _values[3];
	};
}
