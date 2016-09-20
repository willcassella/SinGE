// Vec2.cpp

#include "../../include/Core/Math/Vec2.h"

SGE_REFLECT_TYPE(sge::Vec2)
.implements<sge::IToString>()
.property("x", &sge::Vec2::x, &sge::Vec2::x, PF_SERIALIZED)
.property("y", &sge::Vec2::y, &sge::Vec2::y, PF_SERIALIZED)
.property("length", &sge::Vec2::length, nullptr)
.property("normalized", &sge::Vec2::normalized, nullptr);

namespace sge
{
	const Vec2 Vec2::zero = Vec2{ 0, 0 };
	const Vec2 Vec2::up = Vec2{ 0, 1 };
	const Vec2 Vec2::right = Vec2{ 1, 0 };
}
