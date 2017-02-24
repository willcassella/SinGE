// build.h
#pragma once

#include <Core/env.h>

#if defined SGE_LIGHTMAPPER_BUILD
#   define SGE_LIGHTMAPPER_API SGE_EXPORT
#else
#   define SGE_LIGHTMAPPER_API SGE_IMPORT
#endif
