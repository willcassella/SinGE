// CInput.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Gameplay/CInput.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/EmptyComponentContainer.h"

SGE_REFLECT_TYPE(sge::CInput);

SGE_REFLECT_TYPE(sge::CInput::FActionEvent)
.field_property("name", &FActionEvent::name);

SGE_REFLECT_TYPE(sge::CInput::FAxisEvent)
.field_property("name", &FAxisEvent::name)
.field_property("value", &FAxisEvent::value)
.field_property("max", &FAxisEvent::max);

SGE_REFLECT_TYPE(sge::CInput::FSetAxis)
.field_property("name", &FSetAxis::name)
.field_property("value", &FSetAxis::value);

namespace sge
{
    CInput::CInput(ProcessingFrame& pframe, EntityId entity)
        : TComponentInterface<sge::CInput>(pframe, entity)
    {
    }

    void CInput::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<EmptyComponentContainer<CInput>>());
    }
}
