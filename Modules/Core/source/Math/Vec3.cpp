// Vec3.cpp

#include "../../include/Core/Math/Vec3.h"

SGE_REFLECT_TYPE(sge::Vec3)
.implements<sge::IToString>()
.property("x", &sge::Vec3::x, &sge::Vec3::x, PF_SERIALIZED)
.property("y", &sge::Vec3::y, &sge::Vec3::y, PF_SERIALIZED)
.property("z", &sge::Vec3::z, &sge::Vec3::z, PF_SERIALIZED)
.property("length", &sge::Vec3::length, nullptr)
.property("normalized", &sge::Vec3::normalized, nullptr);

namespace sge
{
	const Vec3 Vec3::zero = Vec3{ 0, 0, 0 };
	const Vec3 Vec3::up = Vec3{ 0, 1, 0 };
	const Vec3 Vec3::forward = Vec3{ 0, 0, -1 };
	const Vec3 Vec3::right = Vec3{ 1, 0, 0 };
}
