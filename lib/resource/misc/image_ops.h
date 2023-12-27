#pragma once

#include "lib/resource/build.h"

namespace sge
{
    namespace image_ops
    {
        /**
         * \brief Saves the given RGB float image to a file (EXR).
         * \param image The image to save.
         * \param width The width of the image (pixels).
         * \param height The height of the image (pixels).
         * \param num_channels The number of channels in each pixel.
         * \param path The path to where the file should be saved (Should end in '.exr').
         */
        SGE_RESOURCE_API void save_rgbf(
            const float* image,
            int32 width,
            int32 height,
            byte num_channels,
            const char* path);

        /**
         * \brief Saves the given RGB float image to a memory buffer.
         * \param image The image to save.
         * \param width The width of the image (pixels).
         * \param height The height of the image (pixels).
         * \param num_channels The number of channels in each pixel.
         * \param out_buff Output pointer to the buffer where the image is saved. This must be freed with 'sge::free'.
         * \param out_size The size of the created buffer.
         */
        SGE_RESOURCE_API void save_rgbf_to_memory(
            const float* image,
            int32 width,
            int32 height,
            byte num_channels,
            byte** out_buff,
            std::size_t* out_size);

        /**
         * \brief Loads an RGB float image from a memory buffer.
         * \param buff The buffer to load the image from.
         * \param size The size of the memory buffer.
         * \param out_image Output pointer to the image. This must be freed with 'sge::free'.
         * \param out_width The width of the image (pixels).
         * \param out_height The height of the image (pixels).
         * \param out_num_channels The number of channels in the image.
         */
        SGE_RESOURCE_API void load_rgbf_from_memory(
            const byte* buff,
            std::size_t size,
            float** out_image,
            int32* out_width,
            int32* out_height,
            byte* out_num_channels);

        /**
         * \brief Performs a dilation filter on the given RGB float image.
         * \param image The image to dilate.
         * \param width The width of the image (pixels).
         * \param height The height of the image (pixels).
         * \param num_channels The number of channels in each pixels.
         * \param out A buffer to store the result of the dilation. This must be the same size as the given image.
         */
        SGE_RESOURCE_API void dilate_rgbf(
            const float* image,
            int32 width,
            int32 height,
            byte num_channels,
            float* out);

        /**
         * \brief Performs a smooth filter on the given RGB float image.
         * \param image The image to smooth.
         * \param width The width of the image (pixels).
         * \param height The height of the image (pixels).
         * \param num_channels The number of channels in each pixel.
         * \param out A buffer to store the result of the smooth filter. This must be the same size as the given image.
         */
        SGE_RESOURCE_API void smooth_rgbf(
            const float* image,
            int32 width,
            int32 height,
            byte num_channels,
            float* out);
    }
}
