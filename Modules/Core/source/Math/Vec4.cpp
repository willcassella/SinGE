// Vec4.cpp

#include "../../include/Core/Math/Vec4.h"

SGE_REFLECT_TYPE(sge::Vec4)
.implements<sge::IToString>()
.property("x", &sge::Vec4::x, &sge::Vec4::x, PF_SERIALIZED)
.property("y", &sge::Vec4::y, &sge::Vec4::y, PF_SERIALIZED)
.property("z", &sge::Vec4::z, &sge::Vec4::z, PF_SERIALIZED)
.property("w", &sge::Vec4::w, &sge::Vec4::w, PF_SERIALIZED)
.property("length", &sge::Vec4::length, nullptr)
.property("normalized", &sge::Vec4::normalized, nullptr);

namespace sge
{
	const Vec4 Vec4::zero = Vec4{ 0, 0, 0, 0 };
}
