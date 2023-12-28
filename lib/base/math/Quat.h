#pragma once

#include "lib/base/math/vec3.h"
#include "lib/base/math/angle.h"

namespace sge
{
    struct SGE_BASE_EXPORT Quat
    {
        SGE_REFLECTED_TYPE;

        constexpr Quat()
            : _values{0, 0, 0, 1}
        {
        }

        Quat(float x, float y, float z, float w)
            : _values{x, y, z, w}
        {
        }

        /** Constructs a new Quaternion
         * 'axis' - The axis about which this rotation represents
         * 'angle' - The amount to rotate about 'axis' */
        Quat(const Vec3 &axis, Angle angle)
        {
            // Make sure the axis vector is normalized
            const auto normAxis = axis.normalized();
            const auto sinHalfAngle = sinf(angle * float{0.5});

            x(normAxis.x() * sinHalfAngle);
            y(normAxis.y() * sinHalfAngle);
            z(normAxis.z() * sinHalfAngle);
            w(cosf(angle * float{0.5}));
        }

        float x() const
        {
            return _values[0];
        }
        void x(float x)
        {
            _values[0] = x;
        }
        float y() const
        {
            return _values[1];
        }
        void y(float y)
        {
            _values[1] = y;
        }
        float z() const
        {
            return _values[2];
        }
        void z(float z)
        {
            _values[2] = z;
        }
        float w() const
        {
            return _values[3];
        }
        void w(float w)
        {
            _values[3] = w;
        }

        /** Formats this Quat as a String */
        std::string to_string() const
        {
            return format("<@, @, @, @>", x(), y(), z(), w());
        }

        /* Serializes the state of this Quat to an archive. */
        void to_archive(ArchiveWriter &writer) const
        {
            writer.typed_array(_values, 4);
        }

        /* Deserializes the state of this Quat from an archive. */
        void from_archive(ArchiveReader &reader)
        {
            reader.typed_array(_values, 4);
        }

        /** Rotate this quaternion around an axis by a certain angle */
        void rotate_by_axis_angle(const Vec3 &axis, Angle angle, bool local)
        {
            // Construct a quaternion from the axis and angle
            Quat rotation{axis, angle};

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

        friend Quat operator*(const Quat &lhs, const Quat &rhs)
        {
            Quat total;
            total.w(lhs.w() * rhs.w() - lhs.x() * rhs.x() - lhs.y() * rhs.y() - lhs.z() * rhs.z());
            total.x(lhs.w() * rhs.x() + lhs.x() * rhs.w() + lhs.y() * rhs.z() - lhs.z() * rhs.y());
            total.y(lhs.w() * rhs.y() - lhs.x() * rhs.z() + lhs.y() * rhs.w() + lhs.z() * rhs.x());
            total.z(lhs.w() * rhs.z() + lhs.x() * rhs.y() - lhs.y() * rhs.x() + lhs.z() * rhs.w());

            return total;
        }
        friend Quat operator*(const Quat &lhs, float rhs)
        {
            Quat total;
            total.w(lhs.w() * rhs);
            total.x(lhs.x() * rhs);
            total.y(lhs.y() * rhs);
            total.z(lhs.z() * rhs);
            return total;
        }
        friend Quat operator+(const Quat &lhs, const Quat &rhs)
        {
            Quat total;
            total.w(lhs.w() + rhs.w());
            total.x(lhs.x() + rhs.x());
            total.y(lhs.y() + rhs.y());
            total.z(lhs.z() + rhs.z());
            return total;
        }
        friend Quat operator-(const Quat &lhs, const Quat &rhs)
        {
            Quat total;
            total.w(lhs.w() - rhs.w());
            total.x(lhs.x() - rhs.x());
            total.y(lhs.y() - rhs.y());
            total.z(lhs.z() - rhs.z());
            return total;
        }
        friend Quat &operator*=(Quat &lhs, const Quat &rhs)
        {
            lhs = lhs * rhs;
            return lhs;
        }
        friend bool operator==(const Quat &lhs, const Quat &rhs)
        {
            return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
        }
        friend bool operator!=(const Quat &lhs, const Quat &rhs)
        {
            return lhs.x() != rhs.x() || lhs.y() != rhs.y() || lhs.z() != rhs.z();
        }

    private:
        float _values[4];
    };
}
