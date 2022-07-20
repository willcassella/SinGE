// from_archive.cpp

#include "base/interfaces/from_archive.h"
#include "base/reflection/ReflectionBuilder.h"

SGE_REFLECT_INTERFACE(sge::IFromArchive)
    .implemented_for<bool>()
    .implemented_for<int8>()
    .implemented_for<uint8>()
    .implemented_for<int16>()
    .implemented_for<uint16>()
    .implemented_for<int32>()
    .implemented_for<uint32>()
    .implemented_for<int64>()
    .implemented_for<uint64>()
    .implemented_for<float>()
    .implemented_for<double>()
    .implemented_for<std::string>();
