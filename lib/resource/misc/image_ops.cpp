#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <FreeImage.h>

#include "lib/base/memory/functions.h"
#include "lib/resource/misc/image_ops.h"

namespace sge
{
    void image_ops::save_rgbf(
        const float* image,
        int32_t width,
        int32_t height,
        uint8_t num_channels,
        const char* path)
    {
        FREE_IMAGE_TYPE type = FIT_UNKNOWN;
        switch (num_channels)
        {
        case 3:
            type = FIT_RGBF;
            break;

        case 4:
            type = FIT_RGBAF;
            break;

        default:
            assert(false);
        }

        auto* bitmap = FreeImage_AllocateT(type, width, height, 32 * num_channels);
        memcpy(FreeImage_GetBits(bitmap), image, width * height * num_channels * sizeof(float));
        FreeImage_Save(FIF_EXR, bitmap, path);
        FreeImage_Unload(bitmap);
    }

    void image_ops::save_rgbf_to_memory(
        const float* image,
        int32_t width,
        int32_t height,
        uint8_t num_channels,
        uint8_t** out_buff,
        size_t* out_size)
    {
        FREE_IMAGE_TYPE type = FIT_UNKNOWN;
        switch (num_channels)
        {
        case 3:
            type = FIT_RGBF;
            break;

        case 4:
            type = FIT_RGBAF;
            break;

        default:
            assert(false);
        }

        // Load the image data into a FreeImage bitmap
        auto* bitmap = FreeImage_AllocateT(type, width, height, 32 * num_channels);
        memcpy(FreeImage_GetBits(bitmap), image, width * height * num_channels * sizeof(float));

        // Save the bitmap to a FreeImage memory stream
        auto* fi_buff = FreeImage_OpenMemory();
        FreeImage_SaveToMemory(FIF_EXR, bitmap, fi_buff);

        // Unload the bitmap
        FreeImage_Unload(bitmap);

        // Acqure the memory stream created by FreeImage
        uint8_t* buff;
        DWORD size;
        FreeImage_AcquireMemory(fi_buff, &buff, &size);

        // Create a buffer for the user of the same size
        uint8_t* user_buff = (uint8_t*)sge::malloc(size);
        memcpy(user_buff, buff, size);

        // Close the FreeImage memory stream
        FreeImage_CloseMemory(fi_buff);

        *out_buff = user_buff;
        *out_size = size;
    }

    void image_ops::load_rgbf_from_memory(
        const uint8_t* buff,
        size_t size,
        float** out_image,
        int32_t* out_width,
        int32_t* out_height,
        uint8_t* out_num_channels)
    {
        // Open the memory with FreeImage
        auto* fi_buff = FreeImage_OpenMemory(const_cast<uint8_t*>(buff), static_cast<DWORD>(size));

        // Open the bitmap with FreeImage
        auto* bitmap = FreeImage_LoadFromMemory(FIF_EXR, fi_buff);

        // Get width and height
        auto width = FreeImage_GetWidth(bitmap);
        auto height = FreeImage_GetHeight(bitmap);

        // Get the number of channels
        uint8_t num_channels = 0;
        const auto image_type = FreeImage_GetImageType(bitmap);
        switch (image_type)
        {
        case FIT_RGBF:
            num_channels = 3;
            break;
        case FIT_RGBAF:
            num_channels = 4;
            break;
        default:
            assert(false);
        }

        // Copy the image into a user buffer
        auto* user_buff = (float*)sge::malloc(width * height * num_channels * sizeof(float));
        memcpy(user_buff, FreeImage_GetBits(bitmap), width * height * num_channels * sizeof(float));

        // Close the bitmap and file stream
        FreeImage_Unload(bitmap);
        FreeImage_CloseMemory(fi_buff);

        *out_image = user_buff;
        *out_width = width;
        *out_height = height;
        *out_num_channels = num_channels;
    }

    void image_ops::dilate_rgbf(
        const float* image,
        int32_t width,
        int32_t height,
        uint8_t num_channels,
        float* out)
    {
        for (int32_t y = 0; y < height; ++y)
        {
            for (int32_t x = 0; x < width; ++x)
            {
                float color[4];
                bool valid = false;

                for (uint8_t channel = 0; channel < num_channels; ++channel)
                {
                    color[channel] = image[(y * width + x) * num_channels + channel];
                    valid |= color[channel] > 0.0f;
                }

                if (!valid)
                {
                    int32_t num_valid = 0;
                    const int32_t dx[] = { -1, 0, 1,  0 };
                    const int32_t dy[] = { 0, 1, 0, -1 };
                    for (uint8_t d = 0; d < 4; ++d)
                    {
                        const int32_t current_x = x + dx[d];
                        const int32_t current_y = y + dy[d];

                        // Make sure this pixel is within the image
                        if (current_x >= 0 && current_x < width && current_y >= 0 && current_y < height)
                        {
                            float dcolor[4];
                            bool dvalid = false;

                            for (uint8_t channel = 0; channel < num_channels; ++channel)
                            {
                                dcolor[channel] = image[(current_x + current_y * width) * num_channels + channel];
                                dvalid |= dcolor[channel] > 0.0f;
                            }

                            if (dvalid)
                            {
                                for (uint8_t channel = 0; channel < num_channels; ++channel)
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

                        for (uint8_t channel = 0; channel < num_channels; ++channel)
                        {
                            color[channel] *= in;
                        }
                    }
                }

                for (uint8_t channel = 0; channel < num_channels; ++channel)
                {
                    out[(x + y * width) * num_channels + channel] = color[channel];
                }
            }
        }
    }

    void image_ops::smooth_rgbf(
        const float* image,
        int32_t width,
        int32_t height,
        uint8_t num_channels,
        float* out)
    {
        for (int32_t y = 0; y < height; ++y)
        {
            for (int32_t x = 0; x < width; ++x)
            {
                float color[4] = {};

                int32_t num_valid = 0;
                for (int32_t dy = -1; dy <= 1; ++dy)
                {
                    const int32_t current_y = y + dy;
                    for (int32_t dx = -1; dx <= 1; ++dx)
                    {
                        const int32_t current_x = x + dx;

                        // Make sure this pixel is within the bounds of the image
                        if (current_x >= 0 && current_x < width && current_y >= 0 && current_y < height)
                        {
                            bool valid = false;
                            for (uint8_t channel = 0; channel < num_channels; ++channel)
                            {
                                // The pixel is valid if it has a non-zero channel
                                valid |= image[(current_x + current_y * width) * num_channels + channel] > 0.0f;
                            }

                            if (valid)
                            {
                                for (uint8_t channel = 0; channel < num_channels; ++channel)
                                {
                                    color[channel] += image[(current_x + current_y * width) * num_channels + channel];
                                }

                                ++num_valid;
                            }
                        }
                    }
                }

                for (uint8_t channel = 0; channel < num_channels; ++channel)
                {
                    out[(x + y * width) * num_channels + channel] = num_valid ? color[channel] / num_valid : 0.0f;
                }
            }
        }
    }
}
