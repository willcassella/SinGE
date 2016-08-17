// config.h
#pragma once

#include "env.h"

#ifdef CORE_BUILD
#	define CORE_API EXPORT
#else
#	define CORE_API IMPORT
#endif
