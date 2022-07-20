// Angle.h
#pragma once

#include "base/reflection/Reflection.h"
#include "base/interfaces/to_archive.h"
#include "base/interfaces/from_archive.h"

namespace sge
{
    /** An arithmetic value representing an angle (internally stored as radians). */
    struct SGE_BASE_EXPORT Angle
    {
        SGE_REFLECTED_TYPE;

        /////////////////////
        ///   Constants   ///
    public:
        /** Multiplication factor for converting degrees to radians. */
        static constexpr Scalar DEGREES_TO_RADIANS = 0.0174533f;

        /** Multiplication factor for converting radians to degrees. */
        static constexpr Scalar RADIANS_TO_DEGREES = 57.2958f;

        ////////////////////////
        ///   Constructors   ///
    public:
        constexpr Angle()
            : _radians{0}
        {
        }
        constexpr Angle(Scalar radians)
            : _radians{radians}
        {
        }

        ///////////////////
        ///   Methods   ///
    public:
        void to_archive(ArchiveWriter &writer) const
        {
            writer.number(_radians);
        }

        void from_archive(ArchiveReader &reader)
        {
            reader.number(_radians);
        }

        /** Returns the current value in radians. */
        Scalar radians() const
        {
            return _radians;
        }

        /** Sets the current value in radians. */
        void radians(Scalar value)
        {
            _radians = value;
        }

        /** Returns the current value in degrees. */
        Scalar degrees() const
        {
            return _radians * RADIANS_TO_DEGREES;
        }

        /** Sets the current value in degrees. */
        void degrees(Scalar value)
        {
            _radians = value * DEGREES_TO_RADIANS;
        }

        /////////////////////
        ///   Operators   ///
    public:
        /** Converts this object to the underlying type. */
        operator Scalar() const
        {
            return _radians;
        }

        //////////////////
        ///   Fields   ///
    private:
        Scalar _radians;
    };

    /////////////////////
    ///   Functions   ///

    /** Constructs an angle in radians. */
    constexpr Angle radians(Scalar value)
    {
        return {value};
    }

    /** Constructs an angle in degrees. */
    constexpr Angle degrees(Scalar value)
    {
        return {value * Angle::DEGREES_TO_RADIANS};
    }
}
