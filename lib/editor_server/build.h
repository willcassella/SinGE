#pragma once

#include "lib/base/env.h"

#ifdef SGE_EDITORSERVERSYSTEM_BUILD
#   define SGE_EDITORSERVERSYSTEM_API SGE_EXPORT
#else
#   define SGE_EDITORSERVERSYSTEM_API SGE_IMPORT
#endif
