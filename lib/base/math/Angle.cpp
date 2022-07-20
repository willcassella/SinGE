// Angle.cpp

#include "base/reflection/ReflectionBuilder.h"
#include "base/math/Angle.h"

SGE_REFLECT_TYPE(sge::Angle)
    .flags(TF_RECURSE_TERMINAL)
    .implements<IToArchive>()
    .implements<IFromArchive>();
