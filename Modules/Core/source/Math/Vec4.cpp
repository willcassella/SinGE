// Vec4.cpp

#include "../../include/Core/Math/Vec4.h"

SGE_REFLECT_TYPE(sge::Vec4)
.implements<IToString>()
.constructor<Scalar, Scalar, Scalar, Scalar>()
.named_constructor<Vec3, Scalar>("xyz_w")
.named_constructor<Scalar, Vec3>("x_yzw")
.named_constructor<Vec2, Vec2>("xy_zw")
.named_constructor<Vec2, Scalar, Scalar>("xy_z_w")
.named_constructor<Scalar, Vec2, Scalar>("x_yz_w")
.named_constructor<Scalar, Scalar, Vec2>("x_y_zw")
.property("x", &Vec4::x, &Vec4::x, PF_SERIALIZED)
.property("y", &Vec4::y, &Vec4::y, PF_SERIALIZED)
.property("z", &Vec4::z, &Vec4::z, PF_SERIALIZED)
.property("w", &Vec4::w, &Vec4::w, PF_SERIALIZED)
.property("length", &Vec4::length, nullptr)
.property("normalized", &Vec4::normalized, nullptr);

namespace sge
{
	const Vec4 Vec4::zero = Vec4{ 0, 0, 0, 0 };
}
