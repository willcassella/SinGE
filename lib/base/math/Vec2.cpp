#include "lib/base/math/vec2.h"
#include "lib/base/reflection/reflection_builder.h"

SGE_REFLECT_TYPE(sge::Vec2)
    .implements<IToString>()
    .implements<IToArchive>()
    .implements<IFromArchive>()
    .constructor<Scalar, Scalar>()
    .named_constructor("zero", &zero)
    .named_constructor("up", &up)
    .named_constructor("right", &right)
    .property("x", &Vec2::x, &Vec2::x)
    .property("y", &Vec2::y, &Vec2::y)
    .property("length", &Vec2::length, nullptr)
    .property("normalized", &Vec2::normalized, nullptr);
