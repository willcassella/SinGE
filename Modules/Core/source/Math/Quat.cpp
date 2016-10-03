// Quat.cpp

#include "../../include/Core/Math/Quat.h"

SGE_REFLECT_TYPE(sge::Quat)
.implements<IToString>()
.constructor<Vec3, Scalar>()
.constructor<Scalar, Scalar, Scalar, Scalar>()
.property("x", &Quat::x, &Quat::x)
.property("y", &Quat::y, &Quat::y)
.property("z", &Quat::z, &Quat::z)
.property("w", &Quat::w, &Quat::w);
