#pragma once

#include "lib/base/interfaces/from_archive.h"
#include "lib/base/interfaces/to_archive.h"
#include "lib/base/math/conversions.h"
#include "lib/base/reflection/reflection_builder.h"

namespace sge
{
    template <typename T>
    struct IVec2
    {
        SGE_REFLECTED_TYPE;

        IVec2()
            : _values{0, 0}
        {
        }

        IVec2(T x, T y)
            : _values{x, y}
        {
        }

        static IVec2 zero()
        {
            return IVec2{0, 0};
        }

        void to_archive(ArchiveWriter &writer) const
        {
            writer.typed_array(_values, 2);
        }

        void from_archive(ArchiveReader &reader)
        {
            reader.typed_array(_values, 2);
        }

        const T *vec() const
        {
            return _values;
        }

        T *vec()
        {
            return _values;
        }

        T x() const
        {
            return _values[0];
        }

        void x(T value)
        {
            _values[0] = value;
        }

        T y() const
        {
            return _values[1];
        }

        void y(T value)
        {
            _values[1] = value;
        }

        T z() const
        {
            return _values[2];
        }

        void z(T value)
        {
            _values[2] = value;
        }

        float norm_f32_x() const
        {
            return to_norm_f32(_values[0]);
        }

        void norm_f32_x(float value)
        {
            _values[0] = from_norm_f32<T>(value);
        }

        float norm_f32_y() const
        {
            return to_norm_f32(_values[1]);
        }

        void norm_f32_y(float value)
        {
            _values[1] = from_norm_f32<T>(value);
        }

        friend IVec2 operator+(const IVec2 &lhs, const IVec2 &rhs)
        {
            return IVec2{lhs.x() + rhs.x(), lhs.y() + rhs.y()};
        }
        friend IVec2 operator*(const IVec2 &lhs, float rhs)
        {
            return IVec2{static_cast<T>(lhs.x() * rhs), static_cast<T>(lhs.y() * rhs)};
        }
        friend IVec2 operator*(float lhs, const IVec2 &rhs)
        {
            return rhs * lhs;
        }
        friend bool operator==(const IVec2 &lhs, const IVec2 &rhs)
        {
            return lhs.x() == rhs.x() && lhs.y() == rhs.y();
        }
        friend bool operator!=(const IVec2 &lhs, const IVec2 &rhs)
        {
            return !(lhs == rhs);
        }

    private:
        T _values[2];
    };

    template <typename T>
    SGE_REFLECT_TYPE_TEMPLATE(IVec2, T)
        .template implements<IToArchive>()
        .template implements<IFromArchive>();

    using HalfVec2 = IVec2<int16>;
    using UHalfVec2 = IVec2<uint16>;
}
