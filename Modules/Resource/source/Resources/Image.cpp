// Image.cpp

#include <FreeImage.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Interfaces/IToArchive.h>
#include "../../include/Resource/Resources/Image.h"
#include "../../include/Resource/Interfaces/IFromFile.h"

SGE_REFLECT_TYPE(sge::Image)
.implements<IToArchive>()
.implements<IFromFile>();

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
		from_file(path.c_str());
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

	bool Image::from_file(const char* path)
	{
		// Unload the current image
		FreeImage_Unload(_bitmap);
		_bitmap = nullptr;

		// Open the file
		auto format = FreeImage_GetFileType(path);
		FIBITMAP* image = FreeImage_Load(format, path, 0);

		// Check if the image was loaded successfully
		if (!image)
		{
			return false;
		}

		// Convert to 32 bits
		_bitmap = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(image);
		return false;
	}

	void Image::to_archive(ArchiveWriter& writer) const
	{
		// Save width and height
		writer.object_member("width", get_width());
		writer.object_member("height", get_height());
		
		// Save the bitmap
		writer.push_object_member("bitmap");
		writer.typed_array(get_bitmap(), get_width() * get_height() * 4);
		writer.pop();
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
