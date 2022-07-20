// Vec2.cpp

#include "base/math/Vec2.h"
#include "base/reflection/ReflectionBuilder.h"

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
