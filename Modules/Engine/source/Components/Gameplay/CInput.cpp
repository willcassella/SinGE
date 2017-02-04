// CInput.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Gameplay/CInput.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/EmptyComponentContainer.h"
#include "../../../include/Engine/TagBuffer.h"

SGE_REFLECT_TYPE(sge::CInput);

SGE_REFLECT_TYPE(sge::CInput::FActionEvent);

SGE_REFLECT_TYPE(sge::CInput::FAxisEvent);

SGE_REFLECT_TYPE(sge::CInput::FSetAxis);

namespace sge
{
    void CInput::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<EmptyComponentContainer<CInput>>());
    }

    void CInput::reset()
    {
    }

    void CInput::add_action_event(const FActionEvent& action_event) const
    {
        _action_tags.add_tag(entity(), action_event);
    }

    void CInput::add_axis_event(const FAxisEvent& axis_event) const
    {
        _axis_tags.add_tag(entity(), axis_event);
    }

    void CInput::set_axis(const FSetAxis& set_axis) const
    {
        _set_axis_tags.add_tag(entity(), set_axis);
    }

    void CInput::generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags)
    {
        // Call base implementation
        ComponentInterface::generate_tags(tags);

        // Create tags
        _action_tags.create_buffer(type_info, tags);
        _axis_tags.create_buffer(type_info, tags);
        _set_axis_tags.create_buffer(type_info, tags);
    }
}
