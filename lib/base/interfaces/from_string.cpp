#include <stdint.h>

#include "lib/base/interfaces/from_string.h"
#include "lib/base/reflection/reflection_builder.h"

SGE_REFLECT_INTERFACE(sge::IFromString)
    .implemented_for<int32_t>()
    .implemented_for<uint32_t>()
    .implemented_for<int64_t>()
    .implemented_for<uint64_t>()
    .implemented_for<float>()
    .implemented_for<double>();
