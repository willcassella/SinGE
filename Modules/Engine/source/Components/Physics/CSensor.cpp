// CSensor.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Physics/CSensor.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/EmptyComponentContainer.h"

SGE_REFLECT_TYPE(sge::CSensor);

namespace sge
{
    void CSensor::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<EmptyComponentContainer<CSensor>>());
    }

    void CSensor::reset()
    {
    }
}
