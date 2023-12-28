#pragma once

#include "lib/base/build.h"

#ifdef SGE_ENGINE_BUILD
#define SGE_ENGINE_API SGE_EXPORT
#else
#define SGE_ENGINE_API SGE_IMPORT
#endif
