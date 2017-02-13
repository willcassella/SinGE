// HalfVec3.h
#pragma once

#include "../env.h"
#include "Conversions.h"

namespace sge
{
    struct HalfVec3
    {
        /////////////////////////
        ///   Constructors    ///
    public:

        HalfVec3()
            : _values{ 0, 0, 0 }
        {
        }

        HalfVec3(int16 x, int16 y, int16 z)
            : _values{ x, y, z }
        {
        }

        static HalfVec3 zero()
        {
            return HalfVec3{ 0, 0, 0 };
        }

        ///////////////////
        ///   Methods   ///
    public:

        const int16* vec() const
        {
            return _values;
        }

        int16* vec()
        {
            return _values;
        }

        float norm_f32_x() const
        {
            return to_norm_f32(_values[0]);
        }

        void norm_f32_x(float value)
        {
            _values[0] = from_norm_f32<int16>(value);
        }

        float norm_f32_y() const
        {
            return to_norm_f32(_values[1]);
        }

        void norm_f32_y(float value)
        {
            _values[1] = from_norm_f32<int16>(value);
        }

        float norm_f32_z() const
        {
            return to_norm_f32(_values[2]);
        }

        void norm_f32_z(float value)
        {
            _values[2] = from_norm_f32<int16>(value);
        }

        //////////////////
        ///   Fields   ///
    private:

        int16 _values[3];
    };
}
