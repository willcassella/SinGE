// config.h
#pragma once

#include <Core/env.h>

#ifdef SGE_JAVASCRIPT_BUILD
#   define SGE_JAVASCRIPT_API SGE_EXPORT
#else
#   define SGE_JAVASCRIPT_API SGE_IMPORT
#endif
