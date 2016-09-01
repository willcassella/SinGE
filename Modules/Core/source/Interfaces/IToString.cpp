// IToString.cpp

#include "../../include/Core/Interfaces/IToString.h"

SGE_REFLECT_INTERFACE(sge::IToString)
.implemented_for<sge::byte>()
.implemented_for<sge::int16>()
.implemented_for<sge::uint16>()
.implemented_for<sge::int32>()
.implemented_for<sge::uint32>()
.implemented_for<sge::int64>()
.implemented_for<sge::uint64>()
.implemented_for<float>()
.implemented_for<double>()
.implemented_for<long double>();
