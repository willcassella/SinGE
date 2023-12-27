#pragma once

#include "lib/resource/resources/image.h"

namespace sge
{
    /* Represents a linear image with 3-4 floating point channels per pixel. */
    struct SGE_RESOURCE_API HDRImage
    {
        SGE_REFLECTED_TYPE;

        HDRImage();
        HDRImage(const HDRImage& copy);
        HDRImage(HDRImage&& move);
        ~HDRImage();
        HDRImage& operator=(const HDRImage& copy);
        HDRImage& operator=(HDRImage&& move);

        bool from_file(const char* path);

        byte get_num_channels() const;

        int32 get_width() const;

        int32 get_height() const;

        float* get_bits();

        const float* get_bits() const;

    private:
        FIBITMAP* _bitmap;
        byte _num_channels;
    };
}
