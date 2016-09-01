// config.h
#pragma once

#include <Core/config.h>

#ifdef SGE_ENGINE_BUILD
#	define SGE_ENGINE_API SGE_EXPORT
#else
#	define SGE_ENGINE_API SGE_IMPORT
#endif
