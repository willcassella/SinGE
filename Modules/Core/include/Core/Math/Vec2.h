// Vec2.h
#pragma once

#include <cmath>
#include <string>
#include "../Reflection/Reflection.h"
#include "../Interfaces/IToString.h"

namespace sge
{
	struct SGE_CORE_API Vec2
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		constexpr Vec2()
			: _values{ 0, 0 }
		{
		}
		constexpr Vec2(Scalar x, Scalar y)
			: _values{ x, y }
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

		/** Formats this Vec2 as a String */
		std::string to_string() const
		{
			return format("<@, @>", x(), y());
		}

		/** Returns the length of this vector */
		Scalar length() const
		{
			return std::sqrt(x()*x() + y()*y());
		}

		/** Returns the normalized version of this vector */
		Vec2 normalized() const
		{
			auto len = length();

			if (len != 0)
			{
				return Vec2{ x()/len, y()/len };
			}
			else
			{
				return Vec2::zero;
			}
		}

		/** Returns the dot product of two vectors */
		static Scalar dot(const Vec2& a, const Vec2& b)
		{
			return a.x() * b.x() + a.y() * b.y();
		}

		/** Returns the angle between two vectors */
		static Scalar angle(const Vec2& a, const Vec2& b)
		{
			return std::acos(Vec2::dot(a.normalized(), b.normalized()));
		}

		///////////////////
		///   Statics   ///
	public:

		static const Vec2 zero;
		static const Vec2 up;
		static const Vec2 right;

		/////////////////////
		///   Operators   ///
	public:

		friend Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
		{
			return Vec2{ lhs.x() + rhs.x(), lhs.y() + rhs.y() };
		}
		friend Vec2 operator+(const Vec2& lhs, Scalar rhs)
		{
			return Vec2{ lhs.x() + rhs, lhs.y() + rhs };
		}
		friend Vec2 operator+(Scalar lhs, const Vec2& rhs)
		{
			return Vec2{ lhs + rhs.x(), lhs + rhs.y() };
		}
		friend Vec2& operator+=(Vec2& lhs, const Vec2& rhs)
		{
			lhs = lhs + rhs;
			return lhs;
		}
		friend Vec2& operator+=(Vec2& lhs, Scalar rhs)
		{
			lhs = lhs + rhs;
			return lhs;
		}
		friend Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
		{
			return Vec2{ lhs.x() - rhs.y(), lhs.y() - rhs.y() };
		}
		friend Vec2 operator-(const Vec2& lhs, Scalar rhs)
		{
			return Vec2{ lhs.x() - rhs, lhs.y() - rhs };
		}
		friend Vec2 operator-(Scalar lhs, const Vec2& rhs)
		{
			return Vec2{ lhs - rhs.x(), lhs - rhs.y() };
		}
		friend Vec2& operator-=(Vec2& lhs, const Vec2& rhs)
		{
			lhs = lhs - rhs;
			return lhs;
		}
		friend Vec2& operator-=(Vec2& lhs, Scalar rhs)
		{
			lhs = lhs - rhs;
			return lhs;
		}
		friend Vec2 operator*(const Vec2& lhs, const Vec2& rhs)
		{
			return Vec2{ lhs.x() * rhs.x(), lhs.y() * rhs.y() };
		}
		friend Vec2 operator*(const Vec2& lhs, Scalar rhs)
		{
			return Vec2{ lhs.x() * rhs, lhs.y() * rhs };
		}
		friend Vec2 operator*(Scalar lhs, const Vec2& rhs)
		{
			return Vec2{ lhs * rhs.x(), lhs * rhs.y() };
		}
		friend Vec2& operator*=(Vec2& lhs, const Vec2& rhs)
		{
			lhs = lhs * rhs;
			return lhs;
		}
		friend Vec2& operator*=(Vec2& lhs, Scalar rhs)
		{
			lhs = lhs * rhs;
			return lhs;
		}
		friend Vec2 operator/(const Vec2& lhs, const Vec2& rhs)
		{
			return Vec2{ lhs.x() / rhs.x(), lhs.y() / rhs.y() };
		}
		friend Vec2 operator/(const Vec2& lhs, Scalar rhs)
		{
			return Vec2{ lhs.x() / rhs, lhs.y() / rhs };
		}
		friend Vec2 operator/(Scalar lhs, const Vec2& rhs)
		{
			return Vec2{ lhs / rhs.x(), lhs / rhs.y() };
		}
		friend Vec2& operator/=(Vec2& lhs, const Vec2& rhs)
		{
			lhs = lhs / rhs;
			return lhs;
		}
		friend Vec2& operator/=(Vec2& lhs, Scalar rhs)
		{
			lhs = lhs / rhs;
			return lhs;
		}
		friend bool operator==(const Vec2& lhs, const Vec2& rhs)
		{
			return lhs.x() == rhs.x() && lhs.y() == rhs.y();
		}
		friend bool operator!=(const Vec2& lhs, const Vec2& rhs)
		{
			return lhs.x() != rhs.x() || lhs.y() != rhs.y();
		}

		//////////////////
		///   Fields   ///
	private:

		Scalar _values[2];
	};
}