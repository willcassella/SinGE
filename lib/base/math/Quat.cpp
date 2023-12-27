#include "lib/base/math/quat.h"
#include "lib/base/reflection/reflection_builder.h"

SGE_REFLECT_TYPE(sge::Quat)
    .implements<IToString>()
    .implements<IToArchive>()
    .implements<IFromArchive>()
    .constructor<Vec3, Scalar>()
    .constructor<Scalar, Scalar, Scalar, Scalar>()
    .named_constructor<Vec3, Scalar>("axis_angle")
    .property("x", &Quat::x, &Quat::x)
    .property("y", &Quat::y, &Quat::y)
    .property("z", &Quat::z, &Quat::z)
    .property("w", &Quat::w, &Quat::w);
