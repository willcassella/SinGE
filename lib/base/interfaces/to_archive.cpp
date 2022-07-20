// to_archive.cpp

#include "base/reflection/ReflectionBuilder.h"
#include "base/interfaces/to_archive.h"
#include "base/io/archive.h"

SGE_REFLECT_INTERFACE(sge::IToArchive)
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
