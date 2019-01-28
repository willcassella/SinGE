// config.h
#pragma once

#include "env.h"

#ifdef SGE_CORE_BUILD
#   define SGE_CORE_API SGE_EXPORT
#else
#   define SGE_CORE_API SGE_IMPORT
#endif
