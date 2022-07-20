// from_string.cpp

#include "base/interfaces/from_string.h"
#include "base/reflection/ReflectionBuilder.h"

SGE_REFLECT_INTERFACE(sge::IFromString)
    .implemented_for<int32>()
    .implemented_for<uint32>()
    .implemented_for<int64>()
    .implemented_for<uint64>()
    .implemented_for<float>()
    .implemented_for<double>();
