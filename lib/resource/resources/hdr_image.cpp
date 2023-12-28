#include <stdint.h>

#include <FreeImage.h>

#include "lib/base/reflection/reflection_builder.h"
#include "lib/resource/interfaces/from_file.h"
#include "lib/resource/resources/hdr_image.h"

SGE_REFLECT_TYPE(sge::HDRImage)
.implements<IFromFile>();

namespace sge
{
    HDRImage::HDRImage()
        : _bitmap(nullptr),
        _num_channels(0)
    {
    }

    HDRImage::HDRImage(const HDRImage& copy)
        : HDRImage()
    {
        if (copy._bitmap)
        {
            _bitmap = FreeImage_Clone(copy._bitmap);
            _num_channels = copy._num_channels;
        }
    }

    HDRImage::HDRImage(HDRImage&& move)
    {
        _bitmap = move._bitmap;
        _num_channels = move._num_channels;
        move._bitmap = nullptr;
        move._num_channels = 0;
    }

    HDRImage::~HDRImage()
    {
        if (_bitmap)
        {
            FreeImage_Unload(_bitmap);
        }
    }

    HDRImage& HDRImage::operator=(const HDRImage& copy)
    {
        if (&copy != this)
        {
            if (_bitmap)
            {
                FreeImage_Unload(_bitmap);
                _bitmap = nullptr;
                _num_channels = 0;
            }

            if (copy._bitmap)
            {
                _bitmap = FreeImage_Clone(copy._bitmap);
                _num_channels = copy._num_channels;
            }
        }

        return *this;
    }

    HDRImage& HDRImage::operator=(HDRImage&& move)
    {
        if (&move != this)
        {
            if (_bitmap)
            {
                FreeImage_Unload(_bitmap);
            }

            _bitmap = move._bitmap;
            _num_channels = move._num_channels;
            move._bitmap = nullptr;
            move._num_channels = 0;
        }

        return *this;
    }

    bool HDRImage::from_file(const char* path)
    {
        // Get the FreeImage file type
        auto format = FreeImage_GetFileType(path);
        if (format == FIF_UNKNOWN)
        {
            format = FreeImage_GetFIFFromFilename(path);
        }

        // Load the image
        auto image = FreeImage_Load(format, path);
        if (!image)
        {
            return false;
        }

        // Get the image type
        const auto image_type = FreeImage_GetImageType(image);
        if (image_type == FIT_RGBF)
        {
            _bitmap = image;
            _num_channels = 3;
        }
        else if (image_type == FIT_RGBAF)
        {
            _bitmap = image;
            _num_channels = 4;
        }
        else
        {
            // Convert it
            _bitmap = FreeImage_ConvertToRGBAF(image);
            _num_channels = 4;
            FreeImage_Unload(image);
        }

        return true;
    }

    uint8_t HDRImage::get_num_channels() const
    {
        return _num_channels;
    }

    int32_t HDRImage::get_width() const
    {
        return FreeImage_GetWidth(_bitmap);
    }

    int32_t HDRImage::get_height() const
    {
        return FreeImage_GetHeight(_bitmap);
    }

    float* HDRImage::get_bits()
    {
        return reinterpret_cast<float*>(FreeImage_GetBits(_bitmap));
    }

    const float* HDRImage::get_bits() const
    {
        return reinterpret_cast<const float*>(FreeImage_GetBits(_bitmap));
    }
}
