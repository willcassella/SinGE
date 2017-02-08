// CLightMaskVolume.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CLightMaskVolume.h"
#include "../../../include/Engine/Util/EmptyComponentContainer.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CLightMaskVolume);

namespace sge
{
    void CLightMaskVolume::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<EmptyComponentContainer<CLightMaskVolume>>());
    }

    void CLightMaskVolume::reset()
    {
    }
}
