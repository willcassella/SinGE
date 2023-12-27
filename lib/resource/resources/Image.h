#pragma once

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

        uint32 get_width() const;

        uint32 get_height() const;

        byte* get_bitmap();

        const byte* get_bitmap() const;

    private:
        FIBITMAP* _bitmap;
    };
}
