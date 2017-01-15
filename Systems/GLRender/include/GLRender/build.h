// build.h
#pragma once

#include <Core/env.h>

#ifdef SGE_GLRENDER_BUILD
#	define SGE_GLRENDER_API SGE_EXPORT
#else
#	define SGE_GLRENDER_API SGE_IMPORT
#endif
