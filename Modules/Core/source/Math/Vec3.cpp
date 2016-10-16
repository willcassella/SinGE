// Vec3.cpp

#include "../../include/Core/Math/Vec3.h"
#include "../../include/Core/Reflection/ReflectionBuilder.h"

SGE_REFLECT_TYPE(sge::Vec3)
.implements<IToString>()
.constructor<Scalar, Scalar, Scalar>()
.named_constructor<Vec2, Scalar>("xy_z")
.named_constructor<Scalar, Vec2>("x_yz")
.property("x", &Vec3::x, &Vec3::x)
.property("y", &Vec3::y, &Vec3::y)
.property("z", &Vec3::z, &Vec3::z)
.property("length", &Vec3::length, nullptr)
.property("normalized", &Vec3::normalized, nullptr);

namespace sge
{
	const Vec3 Vec3::zero = Vec3{ 0, 0, 0 };
	const Vec3 Vec3::up = Vec3{ 0, 1, 0 };
	const Vec3 Vec3::forward = Vec3{ 0, 0, -1 };
	const Vec3 Vec3::right = Vec3{ 1, 0, 0 };
}
