// Quat.cpp

#include "../../include/Core/Math/Quat.h"

SGE_REFLECT_TYPE(sge::Quat)
.implements<sge::IToString>()
.property("x", &sge::Quat::x, &sge::Quat::x)
.property("y", &sge::Quat::y, &sge::Quat::y)
.property("z", &sge::Quat::z, &sge::Quat::z)
.property("w", &sge::Quat::w, &sge::Quat::w);
