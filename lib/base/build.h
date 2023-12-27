#pragma once

#include "lib/base/env.h"

#ifdef SGE_BASE_BUILD
#define SGE_BASE_EXPORT SGE_EXPORT
#else
#define SGE_BASE_EXPORT SGE_IMPORT
#endif
