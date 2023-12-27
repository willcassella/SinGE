#pragma once

#include "lib/base/env.h"

#ifdef SGE_GL_RENDER_BUILD
#   define SGE_GL_RENDER_API SGE_EXPORT
#else
#   define SGE_GL_RENDER_API SGE_IMPORT
#endif
