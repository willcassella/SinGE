#pragma once

#include "lib/base/env.h"

#if defined SGE_BULLET_PHYSICS_BUILD
#define SGE_BULLET_PHYSICS_API SGE_EXPORT
#else
#define SGE_BULLET_PHYSICS_API SGE_IMPORT
#endif
