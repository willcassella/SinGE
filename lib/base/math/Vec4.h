#pragma once

#include "lib/base/math/vec3.h"

namespace sge
{
    struct SGE_BASE_EXPORT Vec4
    {
        SGE_REFLECTED_TYPE;

        constexpr Vec4()
            : _values{0, 0, 0, 0}
        {
        }
        constexpr Vec4(float x, float y, float z, float w)
            : _values{x, y, z, w}
        {
        }
        constexpr Vec4(Vec3 xyz, float w)
            : _values{xyz.x(), xyz.y(), xyz.z(), w}
        {
        }
        constexpr Vec4(float x, Vec3 yzw)
            : _values{x, yzw.x(), yzw.y(), yzw.z()}
        {
        }
        constexpr Vec4(Vec2 xy, Vec2 zw)
            : _values{xy.x(), xy.y(), zw.x(), zw.y()}
        {
        }
        constexpr Vec4(Vec2 xy, float z, float w)
            : _values{xy.x(), xy.y(), z, w}
        {
        }
        constexpr Vec4(float x, Vec2 yz, float w)
            : _values{x, yz.x(), yz.y(), w}
        {
        }
        constexpr Vec4(float x, float y, Vec2 zw)
            : _values{x, y, zw.x(), zw.y()}
        {
        }

        constexpr float x() const
        {
            return _values[0];
        }
        void x(float x)
        {
            _values[0] = x;
        }
        constexpr float y() const
        {
            return _values[1];
        }
        void y(float y)
        {
            _values[1] = y;
        }
        constexpr float z() const
        {
            return _values[2];
        }
        void z(float z)
        {
            _values[2] = z;
        }
        constexpr float w() const
        {
            return _values[3];
        }
        void w(float w)
        {
            _values[3] = w;
        }

        const float *vec() const
        {
            return _values;
        }

        float *vec()
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
        float length() const
        {
            return sqrtf(x() * x() + y() * y() + z() * z() + w() * w());
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
        static float dot(const Vec4 &a, const Vec4 &b)
        {
            return a.x() * b.x() + a.y() * b.y() + a.z() * b.z() + a.w() * b.w();
        }

        /** Returns the angle between two radians */
        static float angle(const Vec4 &a, const Vec4 &b)
        {
            return acosf(Vec4::dot(a.normalized(), b.normalized()));
        }

        static Vec4 zero()
        {
            return {0, 0, 0, 0};
        }

        friend Vec4 operator+(const Vec4 &lhs, const Vec4 &rhs)
        {
            return Vec4{lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z(), lhs.w() + rhs.w()};
        }
        friend Vec4 operator+(const Vec4 &lhs, float rhs)
        {
            return Vec4{lhs.x() + rhs, lhs.y() + rhs, lhs.z() + rhs, lhs.w() + rhs};
        }
        friend Vec4 operator+(float lhs, const Vec4 &rhs)
        {
            return Vec4{lhs + rhs.x(), lhs + rhs.y(), lhs + rhs.z(), lhs + rhs.w()};
        }
        friend Vec4 &operator+=(Vec4 &lhs, const Vec4 &rhs)
        {
            lhs = lhs + rhs;
            return lhs;
        }
        friend Vec4 &operator+=(Vec4 &lhs, float rhs)
        {
            lhs = lhs + rhs;
            return lhs;
        }
        friend Vec4 operator-(const Vec4 &lhs, const Vec4 &rhs)
        {
            return Vec4{lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z(), lhs.w() - rhs.w()};
        }
        friend Vec4 operator-(const Vec4 &lhs, float rhs)
        {
            return Vec4{lhs.x() - rhs, lhs.y() - rhs, lhs.z() - rhs, lhs.w() - rhs};
        }
        friend Vec4 operator-(float lhs, const Vec4 &rhs)
        {
            return Vec4{lhs - rhs.x(), lhs - rhs.y(), lhs - rhs.z(), lhs - rhs.w()};
        }
        friend Vec4 &operator-=(Vec4 &lhs, const Vec4 &rhs)
        {
            lhs = lhs - rhs;
            return lhs;
        }
        friend Vec4 &operator-=(Vec4 &lhs, float rhs)
        {
            lhs = lhs - rhs;
            return lhs;
        }
        friend Vec4 operator*(const Vec4 &lhs, const Vec4 &rhs)
        {
            return Vec4{lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.x() * rhs.x(), lhs.w() * rhs.w()};
        }
        friend Vec4 operator*(const Vec4 &lhs, float rhs)
        {
            return Vec4{lhs.x() * rhs, lhs.y() * rhs, lhs.z() * rhs, lhs.w() * rhs};
        }
        friend Vec4 operator*(float lhs, const Vec4 &rhs)
        {
            return Vec4{lhs * rhs.x(), lhs * rhs.y(), lhs * rhs.z(), lhs * rhs.w()};
        }
        friend Vec4 &operator*=(Vec4 &lhs, const Vec4 &rhs)
        {
            lhs = lhs * rhs;
            return lhs;
        }
        friend Vec4 &operator*=(Vec4 &lhs, float rhs)
        {
            lhs = lhs * rhs;
            return lhs;
        }
        friend Vec4 operator/(const Vec4 &lhs, const Vec4 &rhs)
        {
            return Vec4{lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z(), lhs.w() * rhs.w()};
        }
        friend Vec4 operator/(const Vec4 &lhs, float rhs)
        {
            return Vec4{lhs.x() * rhs, lhs.y() * rhs, lhs.z() * rhs, lhs.w() * rhs};
        }
        friend Vec4 operator/(float lhs, const Vec4 &rhs)
        {
            return Vec4{lhs / rhs.x(), lhs / rhs.y(), lhs / rhs.z(), lhs / rhs.w()};
        }
        friend Vec4 &operator/=(Vec4 &lhs, const Vec4 &rhs)
        {
            lhs = lhs / rhs;
            return lhs;
        }
        friend Vec4 &operator/=(Vec4 &lhs, float rhs)
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

    private:
        float _values[4];
    };
}
