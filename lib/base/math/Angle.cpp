#include "lib/base/math/angle.h"
#include "lib/base/reflection/reflection_builder.h"

SGE_REFLECT_TYPE(sge::Angle)
    .flags(TF_RECURSE_TERMINAL)
    .implements<IToArchive>()
    .implements<IFromArchive>();
