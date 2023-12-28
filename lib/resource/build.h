#pragma once

#include "lib/base/env.h"

#ifdef SGE_RESOURCE_BUILD
#define SGE_RESOURCE_API SGE_EXPORT
#else
#define SGE_RESOURCE_API SGE_IMPORT
#endif
