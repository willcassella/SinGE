#include "lib/base/math/vec4.h"
#include "lib/base/reflection/reflection_builder.h"

SGE_REFLECT_TYPE(sge::Vec4)
    .implements<IToString>()
    .implements<IToArchive>()
    .implements<IFromArchive>()
    .constructor<Scalar, Scalar, Scalar, Scalar>()
    .named_constructor<Vec3, Scalar>("xyz_w")
    .named_constructor<Scalar, Vec3>("x_yzw")
    .named_constructor<Vec2, Vec2>("xy_zw")
    .named_constructor<Vec2, Scalar, Scalar>("xy_z_w")
    .named_constructor<Scalar, Vec2, Scalar>("x_yz_w")
    .named_constructor<Scalar, Scalar, Vec2>("x_y_zw")
    .named_constructor("zero", &zero)
    .property("x", &Vec4::x, &Vec4::x)
    .property("y", &Vec4::y, &Vec4::y)
    .property("z", &Vec4::z, &Vec4::z)
    .property("w", &Vec4::w, &Vec4::w)
    .property("length", &Vec4::length, nullptr)
    .property("normalized", &Vec4::normalized, nullptr);
