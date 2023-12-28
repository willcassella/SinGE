#pragma once

#include <stdint.h>

#include "lib/base/reflection/reflection.h"
#include "lib/resource/misc/color.h"

struct FIBITMAP;

namespace sge
{
    class ArchiveWriter;

    struct SGE_RESOURCE_API Image
    {
        SGE_REFLECTED_TYPE;

        Image();
        Image(const std::string& path);
        Image(const Image& copy);
        Image(Image&& move);
        ~Image();

        bool from_file(const char* path);

        void to_archive(ArchiveWriter& archive) const;

        uint32_t get_width() const;

        uint32_t get_height() const;

        uint8_t* get_bitmap();

        const uint8_t* get_bitmap() const;

    private:
        FIBITMAP* _bitmap;
    };
}
