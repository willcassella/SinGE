// IVec3.h
#pragma once

#include "../env.h"
#include "Conversions.h"

namespace sge
{
    template <typename T>
    struct IVec3
    {
        /////////////////////////
        ///   Constructors    ///
    public:

        IVec3()
            : _values{ 0, 0, 0 }
        {
        }

        IVec3(T x, T y, T z)
            : _values{ x, y, z }
        {
        }

        static IVec3 zero()
        {
            return IVec3{ 0, 0, 0 };
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

        float norm_f32_z() const
        {
            return to_norm_f32(_values[2]);
        }

        void norm_f32_z(float value)
        {
            _values[2] = from_norm_f32<T>(value);
        }

        //////////////////
        ///   Fields   ///
    private:

        T _values[3];
    };

    using HalfVec3 = IVec3<int16>;
    using UHalfVec3 = IVec3<uint16>;
}
