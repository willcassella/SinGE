#pragma once

#include "lib/base/reflection/reflection.h"
#include "lib/resource/build.h"

namespace sge
{
    class ArchiveWriter;
    class ArchiveReader;

    namespace color
    {
        /**
         * \brief Represents a 32-bit color in the RGBA8 format.
         */
        struct SGE_RESOURCE_API RGBA8
        {
            using Color_t = uint32;
            using Red_t = uint8;
            using Green_t = uint8;
            using Blue_t = uint8;
            using Alpha_t = uint8;
            SGE_REFLECTED_TYPE;

            RGBA8()
                : color(0)
            {
            }

            RGBA8(Color_t color)
                : color(color)
            {
            }

            static RGBA8 black()
            {
                return RGBA8{ 0x000000FF };
            }

            static RGBA8 white()
            {
                return RGBA8{ 0xFFFFFFFF };
            }

            std::string to_string() const;

            std::size_t from_string(const char* str, std::size_t len);

            void to_archive(ArchiveWriter& writer) const;

            void from_archive(ArchiveReader& reader);

            Red_t red() const
            {
                return (color >> 24) & 0x000000FF;
            }

            void red(Red_t value)
            {
                color &= 0x00FFFFFF;
                color |= static_cast<Color_t>(value) << 24;
            }

            Green_t green() const
            {
                return (color >> 16) & 0x000000FF;
            }

            void green(Green_t value)
            {
                color &= 0xFF00FFFF;
                color |= static_cast<Color_t>(value) << 16;
            }

            Blue_t blue() const
            {
                return (color >> 8) & 0x000000FF;
            }

            void blue(Blue_t value)
            {
                color &= 0xFFFF00FF;
                color |= static_cast<Color_t>(value) << 8;
            }

            Alpha_t alpha() const
            {
                return color & 0x000000FF;
            }

            void alpha(Alpha_t value)
            {
                color &= 0xFFFFFF00;
                color |= static_cast<Color_t>(value);
            }

            friend bool operator==(const RGBA8& lhs, const RGBA8& rhs)
            {
                return lhs.color == rhs.color;
            }
            friend bool operator!=(const RGBA8& lhs, const RGBA8& rhs)
            {
                return lhs.color != rhs.color;
            }

            Color_t color;
        };

        struct SGE_RESOURCE_API RGBF32
        {
            using Red_t = float;
            using Green_t = float;
            using Blue_t = float;
            SGE_REFLECTED_TYPE;

            RGBF32()
                : _rgb{ 0, 0, 0 }
            {
            }

            RGBF32(float uniform)
                : _rgb{ uniform, uniform, uniform }
            {
            }

            RGBF32(float red, float green, float blue)
                : _rgb{ red, green, blue }
            {
            }

            static RGBF32 black()
            {
                return RGBF32{ 0.f, 0.f, 0.f };
            }

            static RGBF32 white()
            {
                return RGBF32{ 1.f, 1.f, 1.f };
            }

            void to_archive(ArchiveWriter& writer) const;

            void from_archive(ArchiveReader& reader);

            float* vec()
            {
                return _rgb;
            }

            const float* vec() const
            {
                return _rgb;
            }

            Red_t red() const
            {
                return _rgb[0];
            }

            void red(Red_t value)
            {
                _rgb[0] = value;
            }

            Green_t green() const
            {
                return _rgb[1];
            }

            void green(Green_t value)
            {
                _rgb[1] = value;
            }

            Blue_t blue() const
            {
                return _rgb[2];
            }

            void blue(Blue_t value)
            {
                _rgb[2] = value;
            }

            friend RGBF32 operator*(const RGBF32& lhs, float rhs)
            {
                return RGBF32{ lhs.red() * rhs, lhs.green() * rhs, lhs.blue() * rhs };
            }
            friend RGBF32 operator*(float lhs, const RGBF32& rhs)
            {
                return rhs * lhs;
            }
            friend RGBF32 operator*(const RGBF32& lhs, const RGBF32& rhs)
            {
                return RGBF32{ lhs.red() * rhs.red(), lhs.green() * rhs.green(), lhs.blue() * rhs.blue() };
            }
            friend RGBF32 operator/(const RGBF32& lhs, float rhs)
            {
                return RGBF32{ lhs.red() / rhs, lhs.green() / rhs, lhs.blue() / rhs };
            }
            friend RGBF32 operator/(const RGBF32& lhs, const RGBF32& rhs)
            {
                return RGBF32{ lhs.red() / rhs.red(), lhs.green() / rhs.green(), lhs.blue() / rhs.blue() };
            }
            friend RGBF32 operator+(const RGBF32& lhs, const RGBF32& rhs)
            {
                return RGBF32{ lhs.red() + rhs.red(), lhs.green() + rhs.green(), lhs.blue() + rhs.blue() };
            }
            friend RGBF32 operator-(const RGBF32& lhs, const RGBF32& rhs)
            {
                return RGBF32{ lhs.red() - rhs.red(), lhs.green() - rhs.green(), lhs.blue() - rhs.blue() };
            }
            friend RGBF32& operator*=(RGBF32& lhs, float rhs)
            {
                lhs = lhs * rhs;
                return lhs;
            }
            friend RGBF32& operator*=(RGBF32& lhs, const RGBF32& rhs)
            {
                lhs = lhs * rhs;
                return lhs;
            }
            friend RGBF32& operator/=(RGBF32& lhs, float rhs)
            {
                lhs = lhs / rhs;
                return lhs;
            }
            friend RGBF32& operator/=(RGBF32& lhs, const RGBF32& rhs)
            {
                lhs = lhs / rhs;
                return lhs;
            }
            friend RGBF32& operator+=(RGBF32& lhs, const RGBF32& rhs)
            {
                lhs = lhs + rhs;
                return lhs;
            }
            friend RGBF32& operator-=(RGBF32& lhs, const RGBF32& rhs)
            {
                lhs = lhs - rhs;
                return lhs;
            }
            friend bool operator==(const RGBF32& lhs, const RGBF32& rhs)
            {
                bool equal = true;
                equal &= lhs._rgb[0] == rhs._rgb[0];
                equal &= lhs._rgb[1] == rhs._rgb[1];
                equal &= lhs._rgb[2] == rhs._rgb[2];
                return equal;
            }
            friend bool operator!=(const RGBF32& lhs, const RGBF32& rhs)
            {
                return !(lhs == rhs);
            }

        private:
            float _rgb[3];
        };

        struct SGE_RESOURCE_API RGBAF32
        {
            using Red_t = float;
            using Green_t = float;
            using Blue_t = float;
            using Alpha_t = float;
            SGE_REFLECTED_TYPE;

            RGBAF32()
                : _rgba{ 0.f, 0.f, 0.f, 0.f }
            {
            }

            RGBAF32(float uniform)
                : _rgba{ uniform, uniform, uniform, uniform }
            {
            }

            RGBAF32(float red, float green, float blue, float alpha)
                : _rgba{ red, green, blue, alpha }
            {
            }

            static RGBAF32 zero()
            {
                return RGBAF32{ 0.f, 0.f, 0.f, 0.f };
            }

            static RGBAF32 black()
            {
                return RGBAF32{ 0.f, 0.f, 0.f, 1.f };
            }

            static RGBAF32 white()
            {
                return RGBAF32{ 1.f, 1.f, 1.f, 1.f };
            }

            void to_archive(ArchiveWriter& writer) const;

            void from_archive(ArchiveReader& reader);

            float* vec()
            {
                return _rgba;
            }

            const float* vec() const
            {
                return _rgba;
            }

            Red_t red() const
            {
                return _rgba[0];
            }

            void red(Red_t value)
            {
                _rgba[0] = value;
            }

            Green_t green() const
            {
                return _rgba[1];
            }

            void green(Green_t value)
            {
                _rgba[1] = value;
            }

            Blue_t blue() const
            {
                return _rgba[2];
            }

            void blue(Blue_t value)
            {
                _rgba[2] = value;
            }

            Alpha_t alpha() const
            {
                return _rgba[3];
            }

            void alpha(Alpha_t value)
            {
                _rgba[3] = value;
            }

            friend RGBAF32 operator*(const RGBAF32& lhs, float rhs)
            {
                return RGBAF32{ lhs.red() * rhs, lhs.green() * rhs, lhs.blue() * rhs, lhs.alpha() * rhs };
            }
            friend RGBAF32 operator*(float lhs, const RGBAF32& rhs)
            {
                return rhs * lhs;
            }
            friend RGBAF32 operator*(const RGBAF32& lhs, const RGBAF32& rhs)
            {
                return RGBAF32{ lhs.red() * rhs.red(), lhs.green() * rhs.green(), lhs.blue() * rhs.blue(), lhs.alpha() * rhs.alpha() };
            }
            friend RGBAF32 operator/(const RGBAF32& lhs, float rhs)
            {
                return RGBAF32{ lhs.red() / rhs, lhs.green() / rhs, lhs.blue() / rhs, lhs.alpha() / rhs };
            }
            friend RGBAF32 operator/(const RGBAF32& lhs, const RGBAF32& rhs)
            {
                return RGBAF32{ lhs.red() / rhs.red(), lhs.green() / rhs.green(), lhs.blue() / rhs.blue(), lhs.alpha() / rhs.alpha() };
            }
            friend RGBAF32 operator+(const RGBAF32& lhs, const RGBAF32& rhs)
            {
                return RGBAF32{ lhs.red() + rhs.red(), lhs.green() + rhs.green(), lhs.blue() + rhs.blue(), lhs.alpha() + rhs.alpha() };
            }
            friend RGBAF32 operator-(const RGBAF32& lhs, const RGBAF32& rhs)
            {
                return RGBAF32{ lhs.red() - rhs.red(), lhs.green() - rhs.green(), lhs.blue() - rhs.blue(), lhs.alpha() - rhs.alpha() };
            }
            friend RGBAF32& operator*=(RGBAF32& lhs, float rhs)
            {
                lhs = lhs * rhs;
                return lhs;
            }
            friend RGBAF32& operator*=(RGBAF32& lhs, const RGBAF32& rhs)
            {
                lhs = lhs * rhs;
                return lhs;
            }
            friend RGBAF32& operator/=(RGBAF32& lhs, float rhs)
            {
                lhs = lhs / rhs;
                return lhs;
            }
            friend RGBAF32& operator/=(RGBAF32& lhs, const RGBAF32& rhs)
            {
                lhs = lhs / rhs;
                return lhs;
            }
            friend RGBAF32& operator+=(RGBAF32& lhs, const RGBAF32& rhs)
            {
                lhs = lhs + rhs;
                return lhs;
            }
            friend RGBAF32& operator-=(RGBAF32& lhs, const RGBAF32& rhs)
            {
                lhs = lhs - rhs;
                return lhs;
            }
            friend bool operator==(const RGBAF32& lhs, const RGBAF32& rhs)
            {
                bool equal = true;
                equal &= lhs._rgba[0] == rhs._rgba[0];
                equal &= lhs._rgba[1] == rhs._rgba[1];
                equal &= lhs._rgba[2] == rhs._rgba[2];
                equal &= lhs._rgba[3] == rhs._rgba[3];
                return equal;
            }

            friend bool operator!=(const RGBAF32& lhs, const RGBAF32& rhs)
            {
                return !(lhs == rhs);
            }

        private:
            float _rgba[4];
        };
    }
}
