// Vec4.cpp

#include "../../include/Core/Math/Vec4.h"

SGE_REFLECT_TYPE(sge::Vec4)
.implements<sge::IToString>()
.property("x", &sge::Vec4::x, &sge::Vec4::x)
.property("y", &sge::Vec4::y, &sge::Vec4::y)
.property("z", &sge::Vec4::z, &sge::Vec4::z)
.property("w", &sge::Vec4::w, &sge::Vec4::w)
.property("length", &sge::Vec4::length, nullptr, PF_TRANSIENT)
.property("normalized", &sge::Vec4::normalized, nullptr, PF_TRANSIENT);

namespace sge
{
	const Vec4 Vec4::zero = Vec4{ 0, 0, 0, 0 };
}
