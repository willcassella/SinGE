// IFromString.cpp

#include "../../include/Core/Interfaces/IFromString.h"
#include "../../include/Core/Reflection/ReflectionBuilder.h"

SGE_REFLECT_INTERFACE(sge::IFromString)
.implemented_for<int32>()
.implemented_for<int64>()
.implemented_for<uint64>()
.implemented_for<float>()
.implemented_for<double>()
.implemented_for<long double>();
