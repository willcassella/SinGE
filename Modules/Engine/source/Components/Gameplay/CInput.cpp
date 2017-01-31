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
        _ord_action_event_ents.push_back(entity());
        _ord_action_event_tags.push_back(action_event);
    }

    void CInput::add_axis_event(const FAxisEvent& axis_event) const
    {
        _ord_axis_event_ents.push_back(entity());
        _ord_axis_event_tags.push_back(axis_event);
    }

    void CInput::set_axis(const FSetAxis& set_axis) const
    {
        _ord_set_axis_ents.push_back(entity());
        _ord_set_axis_tags.push_back(set_axis);
    }

    void CInput::generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags)
    {
        // Create all of the action event tags
        if (!_ord_action_event_ents.empty())
        {
            tags[&FActionEvent::type_info].push_back(TagBuffer::create(
                type_info,
                _ord_action_event_ents.data(),
                _ord_action_event_tags.data(),
                sizeof(FActionEvent),
                _ord_action_event_tags.size()));
        }

        // Create all of the axis event tags
        if (!_ord_axis_event_ents.empty())
        {
            tags[&FAxisEvent::type_info].push_back(TagBuffer::create(
                type_info,
                _ord_axis_event_ents.data(),
                _ord_axis_event_tags.data(),
                sizeof(FAxisEvent),
                _ord_axis_event_tags.size()));
        }

        // Create all of the set axis tags
        if (!_ord_set_axis_ents.empty())
        {
            tags[&FSetAxis::type_info].push_back(TagBuffer::create(
                type_info,
                _ord_set_axis_ents.data(),
                _ord_set_axis_tags.data(),
                sizeof(FSetAxis),
                _ord_set_axis_tags.size()));
        }
    }
}
