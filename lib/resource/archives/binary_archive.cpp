#include <sys/stat.h>

#include "lib/base/reflection/reflection_builder.h"
#include "lib/base/util/string_utils.h"
#include "lib/resource/archives/binary_archive.h"
#include "lib/resource/archives/binary_archive_reader.h"
#include "lib/resource/archives/binary_archive_writer.h"
#include "lib/resource/interfaces/from_file.h"

SGE_REFLECT_TYPE(sge::BinaryArchive).implements<IFromFile>();

namespace sge {
ArchiveReader* BinaryArchive::read_root() const {
  return new BinaryArchiveReader(_buffer);
}

ArchiveWriter* BinaryArchive::write_root() {
  _buffer.clear();
  return new BinaryArchiveWriter(_buffer);
}

bool BinaryArchive::to_file(const char* path) const {
  // Open the file
  auto* file = fopen(path, "wb");
  if (!file) {
    return false;
  }

  // Write the contents to the file
  fwrite(_buffer.data(), 1, _buffer.size(), file);

  // Close it
  return fclose(file) == 0;
}

bool BinaryArchive::from_file(const char* path) {
  // Make sure the file has the correct extension
  if (!string_ends_with(path, ".sbin")) {
    return false;
  }

  // Clear the buffer
  _buffer.clear();

  // Get the size of the file
  struct stat file_stats;
  if (stat(path, &file_stats) < 0) {
    return false;
  }

  // Open the file
  auto* file = fopen(path, "rb");
  if (!file) {
    return false;
  }

  // Read the contents of the file into the buffer
  _buffer.assign(file_stats.st_size, 0);
  fread(_buffer.data(), 1, _buffer.size(), file);

  // Close the file
  fclose(file);
  return true;
}
}  // namespace sge
