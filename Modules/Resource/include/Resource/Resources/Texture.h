// Texture.h
#pragma once

#include "Image.h"

namespace sge
{
	struct SGE_RESOURCE_API Texture
	{
		SGE_REFLECTED_TYPE;

        enum class ColorSpace
        {
            /**
             * \brief This texture is in sRGB color space, and must be converted to linear color space if transformations are to be performed.
             */
            S_RGB,

            /**
             * \brief This texture is in linear color space, and must be gamma corrected before display.
             */
            RGB,
        };

		////////////////////////
		///   Constructors   ///
	public:

		Texture();
		Texture(const std::string& path);

		///////////////////
		///   Methods   ///
	public:

		bool from_file(const char* path);

		void to_archive(ArchiveWriter& writer) const;

        ColorSpace color_space() const
	    {
            return _color_space;
		}

        void color_space(ColorSpace value)
		{
            _color_space = value;
		}

		//////////////////
		///   Fields   ///
	public:

		Image image;

	private:

        ColorSpace _color_space;
	};
}
