// IVec2.h
#pragma once

#include "../env.h"
#include "Conversions.h"

namespace sge
{
    template <typename T>
    struct IVec2
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        IVec2()
            : _values{ 0, 0 }
        {
        }

        IVec2(T x, T y)
            : _values{ x, y }
        {
        }

        static IVec2 zero()
        {
            return IVec2{ 0, 0 };
        }

        ///////////////////
        ///   Methods   ///
    public:

        const T* vec() const
        {
            return _values;
        }

        T* vec()
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

        /////////////////////
        ///   Operators   ///
    public:

        friend IVec2 operator+(const IVec2& lhs, const IVec2& rhs)
        {
            return IVec2{ lhs.x() + rhs.x(), lhs.y() + rhs.y() };
        }
        friend IVec2 operator*(const IVec2& lhs, float rhs)
        {
            return IVec2{ static_cast<T>(lhs.x() * rhs), static_cast<T>(lhs.y() * rhs) };
        }
        friend IVec2 operator*(float lhs, const IVec2& rhs)
        {
            return rhs * lhs;
        }

        //////////////////
        ///   Fields   ///
    private:

        T _values[2];
    };

    using HalfVec2 = IVec2<int16>;
    using UHalfVec2 = IVec2<uint16>;
}
