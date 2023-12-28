#pragma once

#include "lib/base/build.h"

namespace sge
{
    /* Wrapper for 'malloc', prevents the issue of freeing memory across DLL boundaries. */
    SGE_BASE_EXPORT void *malloc(size_t size);

    /* Wrapper for 'free', prevents the issue of freeing memory across DLL boundaries. */
    SGE_BASE_EXPORT void free(void *buff);

    SGE_BASE_EXPORT void *aligned_alloc(size_t size, size_t alignment);

    SGE_BASE_EXPORT void aligned_free(void *buffer);
}

/* Allocates an array on the stack */
#define SGE_STACK_ALLOC(TYPE, N) static_cast<TYPE *>(alloca(sizeof(TYPE) * (N)))
