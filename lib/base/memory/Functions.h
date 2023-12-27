#pragma once

#include "lib/base/build.h"

namespace sge
{
    /* Wrapper for 'std::malloc', prevents the issue of freeing memory across DLL boundaries. */
    SGE_BASE_EXPORT void *malloc(std::size_t size);

    /* Wrapper for 'std::free', prevents the issue of freeing memory across DLL boundaries. */
    SGE_BASE_EXPORT void free(void *buff);

    SGE_BASE_EXPORT void *aligned_alloc(std::size_t size, std::size_t alignment);

    SGE_BASE_EXPORT void aligned_free(void *buffer);
}

/* Allocates an array on the stack */
#define SGE_STACK_ALLOC(TYPE, N) static_cast<TYPE *>(alloca(sizeof(TYPE) * (N)))
