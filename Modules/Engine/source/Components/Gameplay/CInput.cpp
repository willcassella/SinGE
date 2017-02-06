// CInput.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Gameplay/CInput.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/EmptyComponentContainer.h"
#include "../../../include/Engine/TagBuffer.h"

SGE_REFLECT_TYPE(sge::CInput);

SGE_REFLECT_TYPE(sge::CInput::FActionEvent);

SGE_REFLECT_TYPE(sge::CInput::FAxisEvent);

namespace sge
{
    void CInput::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<EmptyComponentContainer<CInput>>());
    }

    void CInput::reset()
    {
    }

    void CInput::add_action_event(Name_t action_name) const
    {
        _action_tags.add_tag(entity(), FActionEvent{ action_name });
    }

    void CInput::add_axis_event(Name_t axis_name, float value, float min, float max) const
    {
        _axis_tags.add_tag(entity(), FAxisEvent{ axis_name, value, min, max });
    }

    void CInput::generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags)
    {
        // Call base implementation
        ComponentInterface::generate_tags(tags);

        // Create tags
        _action_tags.create_buffer(type_info, tags);
        _axis_tags.create_buffer(type_info, tags);
    }
}
