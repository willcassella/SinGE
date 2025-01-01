#pragma once

#include <stdint.h>

#include "lib/base/reflection/reflection.h"
#include "lib/resource/misc/color.h"

namespace sge {
class ArchiveWriter;

struct SGE_RESOURCE_API Image {
  SGE_REFLECTED_TYPE;

  enum class ColorSpace : uint8_t {
    SRGB = 0,
    Linear = 1,
  };

  Image();
  Image(const std::string& path);
  Image(const Image& copy);
  Image(Image&& move);
  ~Image();

  bool from_file(const char* path);

  uint32_t get_width() const { return width; }
  uint32_t get_height() const { return height; }
  ColorSpace get_colorspace() const { return colorspace; }

  void* get_bitmap() { return bitmap; }
  const void* get_bitmap() const { return bitmap; }

 private:
  uint32_t width = 0;
  uint32_t height = 0;
  ColorSpace colorspace = ColorSpace::SRGB;
  void* bitmap = nullptr;
};
}  // namespace sge
