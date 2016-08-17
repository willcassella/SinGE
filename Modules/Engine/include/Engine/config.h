// config.h
#pragma once

#include <Core/config.h>

#ifdef ENGINE_BUILD
#	define ENGINE_API EXPORT
#else
#	define ENGINE_API IMPORT
#endif
