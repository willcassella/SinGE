#include <stdint.h>

#include "lib/base/interfaces/from_archive.h"
#include "lib/base/reflection/reflection_builder.h"

SGE_REFLECT_INTERFACE(sge::IFromArchive)
    .implemented_for<bool>()
    .implemented_for<int8_t>()
    .implemented_for<uint8_t>()
    .implemented_for<int16_t>()
    .implemented_for<uint16_t>()
    .implemented_for<int32_t>()
    .implemented_for<uint32_t>()
    .implemented_for<int64_t>()
    .implemented_for<uint64_t>()
    .implemented_for<float>()
    .implemented_for<double>()
    .implemented_for<std::string>();
