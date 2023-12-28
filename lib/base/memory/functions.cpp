#include <stdlib.h>

#include "lib/base/memory/functions.h"

void* sge::malloc(size_t size) {
  return ::malloc(size);
}

void sge::free(void* buff) {
  ::free(buff);
}

#if defined SGE_OS_WINDOWS

void* sge::aligned_alloc(size_t size, size_t alignment) {
  return _aligned_malloc(size, alignment);
}

void sge::aligned_free(void* buffer) {
  _aligned_free(buffer);
}

#else

void* sge::aligned_alloc(size_t size, size_t alignment) {
  return ::aligned_alloc(size, alignment);
}

void sge::aligned_free(void* buffer) {
  ::free(buffer);
}

#endif
