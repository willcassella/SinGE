// Color.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "../build.h"

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

			////////////////////////
			///   Constructors   ///
		public:

			RGBA8(Color_t color = 0xFFFFFFFF)
				: color(color)
			{
			}

			///////////////////
			///   Methods   ///
		public:

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

            /////////////////////
            ///   Operators   ///
		public:

            friend bool operator==(const RGBA8& lhs, const RGBA8& rhs)
            {
                return lhs.color == rhs.color;
            }
            friend bool operator!=(const RGBA8& lhs, const RGBA8& rhs)
            {
                return lhs.color != rhs.color;
            }

			//////////////////
			///   Fields   ///
		public:

			Color_t color;
		};
	}
}
