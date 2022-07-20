// Vec4.h
#pragma once

#include "base/math/Vec3.h"

namespace sge
{
    struct SGE_BASE_EXPORT Vec4
    {
        SGE_REFLECTED_TYPE;

        ////////////////////////
        ///   Constructors   ///
    public:
        constexpr Vec4()
            : _values{0, 0, 0, 0}
        {
        }
        constexpr Vec4(Scalar x, Scalar y, Scalar z, Scalar w)
            : _values{x, y, z, w}
        {
        }
        constexpr Vec4(Vec3 xyz, Scalar w)
            : _values{xyz.x(), xyz.y(), xyz.z(), w}
        {
        }
        constexpr Vec4(Scalar x, Vec3 yzw)
            : _values{x, yzw.x(), yzw.y(), yzw.z()}
        {
        }
        constexpr Vec4(Vec2 xy, Vec2 zw)
            : _values{xy.x(), xy.y(), zw.x(), zw.y()}
        {
        }
        constexpr Vec4(Vec2 xy, Scalar z, Scalar w)
            : _values{xy.x(), xy.y(), z, w}
        {
        }
        constexpr Vec4(Scalar x, Vec2 yz, Scalar w)
            : _values{x, yz.x(), yz.y(), w}
        {
        }
        constexpr Vec4(Scalar x, Scalar y, Vec2 zw)
            : _values{x, y, zw.x(), zw.y()}
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
        constexpr Scalar w() const
        {
            return _values[3];
        }
        void w(Scalar w)
        {
            _values[3] = w;
        }

        const Scalar *vec() const
        {
            return _values;
        }

        Scalar *vec()
        {
            return _values;
        }

        /** Formats this Vec4 as a String */
        std::string to_string() const
        {
            return format("<@, @, @, @>", x(), y(), z(), w());
        }

        /* Serializes the state of this Vec4 to an archive. */
        void to_archive(ArchiveWriter &writer) const
        {
            writer.typed_array(_values, 4);
        }

        /* Deserializes the state of this Vec4 from an archive. */
        void from_archive(ArchiveReader &reader)
        {
            reader.typed_array(_values, 4);
        }

        /** Returns the length of this vector */
        Scalar length() const
        {
            return std::sqrt(x() * x() + y() * y() + z() * z() + w() * w());
        }

        /** Normalizes this vector */
        Vec4 normalized() const
        {
            auto len = length();

            if (len != 0)
            {
                return Vec4{x() / len, y() / len, z() / len, w() / len};
            }
            else
            {
                return Vec4::zero();
            }
        }

        /** Returns the dot product of two Vec4s */
        static Scalar dot(const Vec4 &a, const Vec4 &b)
        {
            return a.x() * b.x() + a.y() * b.y() + a.z() * b.z() + a.w() * b.w();
        }

        /** Returns the angle between two radians */
        static Scalar angle(const Vec4 &a, const Vec4 &b)
        {
            return std::acos(Vec4::dot(a.normalized(), b.normalized()));
        }

        ////////////////////////////
        ///   Static Instances   ///
    public:
        static Vec4 zero()
        {
            return {0, 0, 0, 0};
        }

        /////////////////////
        ///   Operators   ///
    public:
        friend Vec4 operator+(const Vec4 &lhs, const Vec4 &rhs)
        {
            return Vec4{lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z(), lhs.w() + rhs.w()};
        }
        friend Vec4 operator+(const Vec4 &lhs, Scalar rhs)
        {
            return Vec4{lhs.x() + rhs, lhs.y() + rhs, lhs.z() + rhs, lhs.w() + rhs};
        }
        friend Vec4 operator+(Scalar lhs, const Vec4 &rhs)
        {
            return Vec4{lhs + rhs.x(), lhs + rhs.y(), lhs + rhs.z(), lhs + rhs.w()};
        }
        friend Vec4 &operator+=(Vec4 &lhs, const Vec4 &rhs)
        {
            lhs = lhs + rhs;
            return lhs;
        }
        friend Vec4 &operator+=(Vec4 &lhs, Scalar rhs)
        {
            lhs = lhs + rhs;
            return lhs;
        }
        friend Vec4 operator-(const Vec4 &lhs, const Vec4 &rhs)
        {
            return Vec4{lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z(), lhs.w() - rhs.w()};
        }
        friend Vec4 operator-(const Vec4 &lhs, Scalar rhs)
        {
            return Vec4{lhs.x() - rhs, lhs.y() - rhs, lhs.z() - rhs, lhs.w() - rhs};
        }
        friend Vec4 operator-(Scalar lhs, const Vec4 &rhs)
        {
            return Vec4{lhs - rhs.x(), lhs - rhs.y(), lhs - rhs.z(), lhs - rhs.w()};
        }
        friend Vec4 &operator-=(Vec4 &lhs, const Vec4 &rhs)
        {
            lhs = lhs - rhs;
            return lhs;
        }
        friend Vec4 &operator-=(Vec4 &lhs, Scalar rhs)
        {
            lhs = lhs - rhs;
            return lhs;
        }
        friend Vec4 operator*(const Vec4 &lhs, const Vec4 &rhs)
        {
            return Vec4{lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.x() * rhs.x(), lhs.w() * rhs.w()};
        }
        friend Vec4 operator*(const Vec4 &lhs, Scalar rhs)
        {
            return Vec4{lhs.x() * rhs, lhs.y() * rhs, lhs.z() * rhs, lhs.w() * rhs};
        }
        friend Vec4 operator*(Scalar lhs, const Vec4 &rhs)
        {
            return Vec4{lhs * rhs.x(), lhs * rhs.y(), lhs * rhs.z(), lhs * rhs.w()};
        }
        friend Vec4 &operator*=(Vec4 &lhs, const Vec4 &rhs)
        {
            lhs = lhs * rhs;
            return lhs;
        }
        friend Vec4 &operator*=(Vec4 &lhs, Scalar rhs)
        {
            lhs = lhs * rhs;
            return lhs;
        }
        friend Vec4 operator/(const Vec4 &lhs, const Vec4 &rhs)
        {
            return Vec4{lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z(), lhs.w() * rhs.w()};
        }
        friend Vec4 operator/(const Vec4 &lhs, Scalar rhs)
        {
            return Vec4{lhs.x() * rhs, lhs.y() * rhs, lhs.z() * rhs, lhs.w() * rhs};
        }
        friend Vec4 operator/(Scalar lhs, const Vec4 &rhs)
        {
            return Vec4{lhs / rhs.x(), lhs / rhs.y(), lhs / rhs.z(), lhs / rhs.w()};
        }
        friend Vec4 &operator/=(Vec4 &lhs, const Vec4 &rhs)
        {
            lhs = lhs / rhs;
            return lhs;
        }
        friend Vec4 &operator/=(Vec4 &lhs, Scalar rhs)
        {
            lhs = lhs / rhs;
            return lhs;
        }
        friend bool operator==(const Vec4 &lhs, const Vec4 &rhs)
        {
            return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z() && lhs.w() == rhs.w();
        }
        friend bool operator!=(const Vec4 &lhs, const Vec4 &rhs)
        {
            return lhs.x() != rhs.x() || lhs.y() != rhs.y() || lhs.z() != rhs.z() || lhs.w() != rhs.w();
        }

        //////////////////
        ///   Fields   ///
    private:
        Scalar _values[4];
    };
}
