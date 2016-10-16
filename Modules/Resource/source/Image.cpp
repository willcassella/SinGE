// Image.cpp

#include <FreeImage.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Resource/Image.h"

SGE_REFLECT_TYPE(sge::Image);

namespace sge
{
	Image::Image()
		: _bitmap(nullptr)
	{
	}

	Image::Image(const std::string& path)
		: Image()
	{
		// Open the file
		auto format = FreeImage_GetFileType(path.c_str());
		FIBITMAP* image = FreeImage_Load(format, path.c_str(), 0);

		if (!image)
		{
			return;
		}

		// Convert to 32 bits
		_bitmap = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(image);
	}

	Image::Image(const Image& copy)
		: Image()
	{
		if (copy._bitmap)
		{
			_bitmap = FreeImage_Clone(copy._bitmap);
		}
	}

	Image::Image(Image&& move)
		: _bitmap(move._bitmap)
	{
		move._bitmap = nullptr;
	}

	Image::~Image()
	{
		FreeImage_Unload(_bitmap);
	}

	uint32 Image::get_width() const
	{
		return FreeImage_GetWidth(_bitmap);
	}

	uint32 Image::get_height() const
	{
		return FreeImage_GetHeight(_bitmap);
	}

	byte* Image::get_bitmap()
	{
		return FreeImage_GetBits(_bitmap);
	}

	const byte* Image::get_bitmap() const
	{
		return FreeImage_GetBits(_bitmap);
	}
}
