#include <sstream>

#include "lib/base/interfaces/from_archive.h"
#include "lib/base/interfaces/from_string.h"
#include "lib/base/interfaces/to_archive.h"
#include "lib/base/interfaces/to_string.h"
#include "lib/base/memory/functions.h"
#include "lib/base/reflection/reflection_builder.h"
#include "lib/resource/misc/color.h"

SGE_REFLECT_TYPE(sge::color::RGBA8)
.flags(TF_RECURSE_TERMINAL)
.implements<IToString>()
.implements<IFromString>()
.implements<IToArchive>()
.implements<IFromArchive>()
.property("red", &RGBA8::red, &RGBA8::red)
.property("green", &RGBA8::green, &RGBA8::green)
.property("blue", &RGBA8::blue, &RGBA8::blue)
.property("alpha", &RGBA8::alpha, &RGBA8::alpha);

SGE_REFLECT_TYPE(sge::color::RGBF32)
.flags(TF_RECURSE_TERMINAL)
.implements<IToArchive>()
.implements<IFromArchive>()
.property("red", &RGBF32::red, &RGBF32::red)
.property("green", &RGBF32::green, &RGBF32::green)
.property("blue", &RGBF32::blue, &RGBF32::blue);

SGE_REFLECT_TYPE(sge::color::RGBAF32)
.flags(TF_RECURSE_TERMINAL)
.implements<IToArchive>()
.implements<IFromArchive>()
.property("red", &RGBAF32::red, &RGBAF32::red)
.property("green", &RGBAF32::green, &RGBAF32::green)
.property("blue", &RGBAF32::blue, &RGBAF32::blue)
.property("alpha", &RGBAF32::alpha, &RGBAF32::alpha);

namespace sge
{
    namespace color
    {
        std::string RGBA8::to_string() const
        {
            std::stringstream result;
            result << std::hex << color;
            return result.str();
        }

        std::size_t RGBA8::from_string(const char* str, std::size_t /*len*/)
        {
            char* end = nullptr;
            color = std::strtoul(str, &end, 16);
            return end - str;
        }

        void RGBA8::to_archive(ArchiveWriter& writer) const
        {
            sge::to_archive(to_string(), writer);
        }

        void RGBA8::from_archive(ArchiveReader& reader)
        {
            // Get the length of the string
            std::size_t len = 0;
            if (!reader.string_size(len))
            {
                return;
            }

            // Create a buffer to hold the string
            char* buff = SGE_STACK_ALLOC(char, len + 1);
            reader.string(buff, len);
            buff[len] = 0;

            // Set the value from the string
            color = std::strtoul(buff, nullptr, 16);
        }

        void RGBF32::to_archive(ArchiveWriter& writer) const
        {
            writer.typed_array(_rgb, 3);
        }

        void RGBF32::from_archive(ArchiveReader& reader)
        {
            reader.typed_array(_rgb, 3);
        }

        void RGBAF32::to_archive(ArchiveWriter& writer) const
        {
            writer.typed_array(_rgba, 4);
        }

        void RGBAF32::from_archive(ArchiveReader& reader)
        {
            reader.typed_array(_rgba, 4);
        }
    }
}
