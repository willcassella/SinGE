// CLightMaskObstructor.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CLightMaskObstructor.h"
#include "../../../include/Engine/Util/EmptyComponentContainer.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CLightMaskObstructor);

namespace sge
{
    void CLightMaskObstructor::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<EmptyComponentContainer<CLightMaskObstructor>>());
    }

    void CLightMaskObstructor::reset()
    {
    }
}
