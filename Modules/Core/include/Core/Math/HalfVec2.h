// HalfVec2.h
#pragma once

#include "../env.h"
#include "Conversions.h"

namespace sge
{
    struct HalfVec2
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        HalfVec2()
            : _values{ 0, 0 }
        {
        }

        HalfVec2(int16 x, int16 y)
            : _values{ x, y }
        {
        }

        static HalfVec2 zero()
        {
            return HalfVec2{ 0, 0 };
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

        //////////////////
        ///   Fields   ///
    private:

        int16 _values[2];
    };
}
