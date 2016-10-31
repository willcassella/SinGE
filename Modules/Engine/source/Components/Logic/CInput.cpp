// CInput.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Logic/CInput.h"

SGE_REFLECT_TYPE(sge::CInput);

SGE_REFLECT_TYPE(sge::CInput::FActionEvent)
.field_property("name", &FActionEvent::name);

SGE_REFLECT_TYPE(sge::CInput::FAxisEvent)
.field_property("name", &FAxisEvent::name)
.field_property("value", &FAxisEvent::value);
