// Functions.h
#pragma once

#include "../config.h"

namespace sge
{
	/* Wrapper for 'std::malloc', prevents the issue of freeing memory across DLL boundaries. */
	SGE_CORE_API void* malloc(std::size_t size);

	/* Wrapper for 'std::free', prevents the issue of freeing memory across DLL boundaries. */
	SGE_CORE_API void free(void* buff);
}
