#include "lib/base/math/vec3.h"
#include "lib/base/reflection/reflection_builder.h"

SGE_REFLECT_TYPE(sge::Vec3)
    .implements<IToString>()
    .implements<IToArchive>()
    .implements<IFromArchive>()
    .constructor<Scalar, Scalar, Scalar>()
    .named_constructor<Vec2, Scalar>("xy_z")
    .named_constructor<Scalar, Vec2>("x_yz")
    .named_constructor("zero", &zero)
    .named_constructor("up", &up)
    .named_constructor("forward", &forward)
    .named_constructor("right", &right)
    .property("x", &Vec3::x, &Vec3::x)
    .property("y", &Vec3::y, &Vec3::y)
    .property("z", &Vec3::z, &Vec3::z)
    .property("length", &Vec3::length, nullptr)
    .property("normalized", &Vec3::normalized, nullptr);
