// Vec2.cpp

#include "../../include/Core/Math/Vec2.h"

SGE_REFLECT_TYPE(sge::Vec2)
.implements<IToString>()
.constructor<Scalar, Scalar>()
.property("x", &Vec2::x, &Vec2::x, PF_SERIALIZED)
.property("y", &Vec2::y, &Vec2::y, PF_SERIALIZED)
.property("length", &Vec2::length, nullptr)
.property("normalized", &Vec2::normalized, nullptr);

namespace sge
{
	const Vec2 Vec2::zero = Vec2{ 0, 0 };
	const Vec2 Vec2::up = Vec2{ 0, 1 };
	const Vec2 Vec2::right = Vec2{ 1, 0 };
}
