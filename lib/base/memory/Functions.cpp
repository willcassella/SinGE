// Functions.cpp

#include <cstdlib>
#include "base/memory/Functions.h"

void *sge::malloc(std::size_t size)
{
    return std::malloc(size);
}

void sge::free(void *buff)
{
    std::free(buff);
}

#if defined SGE_OS_WINDOWS

void *sge::aligned_alloc(std::size_t size, std::size_t alignment)
{
    return _aligned_malloc(size, alignment);
}

void sge::aligned_free(void *buffer)
{
    _aligned_free(buffer);
}

#else

void *sge::aligned_alloc(std::size_t size, std::size_t alignment)
{
    return aligned_alloc(size, alignment);
}

void sge::aligned_free(void *buffer)
{
    std::free(buffer);
}

#endif
