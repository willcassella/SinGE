#include <stddef.h>
#include <stdint.h>

#include "lib/third_party/qoi/qoi.h"

#include "lib/base/memory/functions.h"
#include "lib/base/reflection/reflection_builder.h"
#include "lib/resource/interfaces/from_file.h"
#include "lib/resource/resources/image.h"

SGE_REFLECT_TYPE(sge::Image).implements<IFromFile>();

namespace sge {
static size_t calc_size(uint32_t width, uint32_t height) {
  return width * height * sizeof(uint32_t);
}

Image::Image() = default;

Image::Image(const std::string& path) : Image() {
  // Open the file
  from_file(path.c_str());
}

Image::Image(const Image& copy) : width(copy.width), height(copy.height), colorspace(copy.colorspace) {
  bitmap = malloc(calc_size(width, height));
  memcpy(bitmap, copy.bitmap, calc_size(width, height));
}

Image::Image(Image&& move)
    : width(move.width), height(move.height), colorspace(move.colorspace), bitmap(move.bitmap) {
  move.width = 0;
  move.height = 0;
  move.colorspace = ColorSpace::SRGB;
  move.bitmap = nullptr;
}

Image::~Image() {
  free(bitmap);
}

bool Image::from_file(const char* path) {
  qoi_desc desc;
  void* result = qoi_read(path, &desc, 4);
  if (!result) {
    return false;
  }

  // Unload current image if necessary.
  if (bitmap) {
    free(bitmap);
  }

  width = desc.width;
  height = desc.height;
  colorspace = static_cast<ColorSpace>(desc.colorspace);
  bitmap = result;

  return true;
}
}  // namespace sge
