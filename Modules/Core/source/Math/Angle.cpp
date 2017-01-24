// Angle.cpp

#include "../../include/Core/Reflection/ReflectionBuilder.h"
#include "../../include/Core/Math/Angle.h"

SGE_REFLECT_TYPE(sge::Angle)
.flags(TF_RECURSE_TERMINAL)
.implements<IToArchive>()
.implements<IFromArchive>();
