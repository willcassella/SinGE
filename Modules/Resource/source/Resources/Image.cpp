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
    static void swap_red_and_blue_32(byte* bitmap, std::size_t num_pixels)
    {
        for (std::size_t i = 0; i < num_pixels; ++i)
        {
            const std::size_t pixel_offset = i * 4;
            const byte temp = bitmap[pixel_offset];
            bitmap[pixel_offset] = bitmap[pixel_offset + 2];
            bitmap[pixel_offset + 2] = temp;
        }
    }

	Image::Image()
		: _bitmap(nullptr)
	{
	}

    Image::Image(int32 width, int32 height, ColorFormat format)
    {
        switch (format)
        {
        case ColorFormat::RGBA8:
            _bitmap = FreeImage_AllocateT(FIT_BITMAP, width, height, 32);
            break;

        case ColorFormat::RGB32F:
            _bitmap = FreeImage_AllocateT(FIT_RGBF, width, height, 32 * 3);
            break;
        }
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

    void Image::save_rgbf(const float* image, int32 width, int32 height, const char* path)
    {
        auto* bitmap = FreeImage_AllocateT(FIT_RGBF, width, height, 32 * 3);
        std::memcpy(FreeImage_GetBits(bitmap), image, width * height * 3 * sizeof(float));
        FreeImage_Save(FIF_EXR, bitmap, path);
        FreeImage_Unload(bitmap);
    }

    void Image::dilate_rgbf(const float* image, int32 width, int32 height, byte num_channels, float* out)
    {
        for (int32 y = 0; y < height; ++y)
        {
            for (int32 x = 0; x < width; ++x)
            {
                float color[4];
                bool valid = false;

                for (byte channel = 0; channel < num_channels; ++channel)
                {
                    color[channel] = image[(x + y * width) * num_channels + channel];
                    valid |= color[channel] > 0.0f;
                }

                if (!valid)
                {
                    int32 num_valid = 0;
                    const int32 dx[] = { -1, 0, 1,  0 };
                    const int32 dy[] = { 0, 1, 0, -1 };
                    for (byte d = 0; d < 4; ++d)
                    {
                        const int32 current_x = x + dx[d];
                        const int32 current_y = y + dy[d];

                        // Make sure this pixel is within the image
                        if (current_x >= 0 && current_x < width && current_y >= 0 && current_y < height)
                        {
                            float dcolor[4];
                            bool dvalid = false;

                            for (byte channel = 0; channel < num_channels; ++channel)
                            {
                                dcolor[channel] = image[(current_x + current_y * width) * num_channels + channel];
                                dvalid |= dcolor[channel] > 0.0f;
                            }

                            if (dvalid)
                            {
                                for (byte channel = 0; channel < num_channels; ++num_channels)
                                {
                                    color[channel] += dcolor[channel];
                                }

                                ++num_valid;
                            }
                        }
                    }

                    if (num_valid > 0)
                    {
                        const float in = 1.0f / num_valid;

                        for (byte channel = 0; channel < num_channels; ++num_channels)
                        {
                            color[channel] *= in;
                        }
                    }
                }

                for (byte channel = 0; channel < num_channels; ++channel)
                {
                    out[(x + y * width) * num_channels + channel] = color[channel];
                }
            }
        }
    }

    void Image::smooth_rgbf(const float* image, int32 width, int32 height, byte num_channels, float* out)
    {
        for (int32 y = 0; y < height; ++y)
        {
            for (int32 x = 0; x < width; ++x)
            {
                float color[4];

                int32 num_valid = 0;
                for (int32 dy = -1; dy <= 1; ++dy)
                {
                    const int32 current_y = y + dy;
                    for (int32 dx = -1; dx <= 1; ++dx)
                    {
                        const int32 current_x = x + dx;

                        // Make sure this pixel is within the bounds of the image
                        if (current_x >= 0 && current_x < width && current_y >= 0 && current_y < height)
                        {
                            bool valid = false;
                            for (byte channel = 0; channel < num_channels; ++channel)
                            {
                                // The pixel is valid if it has a non-zero channel
                                valid |= image[(current_x + current_y * width) * num_channels + channel] > 0.0f;
                            }

                            if (valid)
                            {
                                for (byte channel = 0; channel < num_channels; ++channel)
                                {
                                    color[channel] += image[(current_x + current_y * width) * num_channels + channel];
                                }

                                ++num_valid;
                            }
                        }
                    }
                }

                for (byte channel = 0; channel < num_channels; ++channel)
                {
                    out[(x + y * width) * num_channels + channel] = num_valid ? color[channel] / num_valid : 0.0f;
                }
            }
        }
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

        // Swap red and blue if not in the RGB format
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
        swap_red_and_blue_32(get_bitmap(), get_width() * get_height());
#endif
		return true;
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
