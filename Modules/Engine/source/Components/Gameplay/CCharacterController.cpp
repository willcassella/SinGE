// CCharacterController.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Gameplay/CCharacterController.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/TagBuffer.h"

SGE_REFLECT_TYPE(sge::CCharacterController)
.property("step_height", &CCharacterController::step_height, &CCharacterController::step_height)
.property("max_slope", &CCharacterController::max_slope, &CCharacterController::max_slope)
.property("jump_speed", &CCharacterController::jump_speed, &CCharacterController::jump_speed)
.property("fall_speed", &CCharacterController::fall_speed, &CCharacterController::fall_speed)
.property("on_ground", &CCharacterController::on_ground, nullptr, PF_EDITOR_HIDDEN);

SGE_REFLECT_TYPE(sge::CCharacterController::FJumpEvent);

SGE_REFLECT_TYPE(sge::CCharacterController::FWalkEvent)
.field_property("direction", &FWalkEvent::direction);

SGE_REFLECT_TYPE(sge::CCharacterController::FTurnEvent)
.field_property("amount", &FTurnEvent::amount);

namespace sge
{
    struct CCharacterController::Data
    {
        ///////////////////
        ///   Methods   ///
    public:

        void to_archive(ArchiveWriter& writer) const
        {
            writer.object_member("stpht", step_height);
            writer.object_member("mxslp", max_slope);
            writer.object_member("jspd", jump_speed);
            writer.object_member("fspd", fall_speed);
            writer.object_member("grd", on_ground);
        }

        void from_archive(ArchiveReader& reader)
        {
            reader.object_member("stpht", step_height);
            reader.object_member("mxslp", max_slope);
            reader.object_member("jspd", jump_speed);
            reader.object_member("fsdp", fall_speed);
            reader.object_member("grd", on_ground);
        }

        //////////////////
        ///   Fields   ///
    public:

        float step_height = 0.1f;
        Angle max_slope = degrees(30);
        float jump_speed = 10.f;
        float fall_speed = 55.f;
        bool on_ground = true;
    };

    void CCharacterController::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CCharacterController, Data>>());
    }

    void CCharacterController::reset(Data& data)
    {
        _data = &data;
    }

    bool CCharacterController::on_ground() const
    {
        return _data->on_ground;
    }

    void CCharacterController::set_on_ground(bool value)
    {
        _data->on_ground = value;
    }

    float CCharacterController::step_height() const
    {
        return _data->step_height;
    }

    void CCharacterController::step_height(float value)
    {
        checked_setter(value, _data->step_height);
    }

    Angle CCharacterController::max_slope() const
    {
        return _data->max_slope;
    }

    void CCharacterController::max_slope(Angle value)
    {
        checked_setter(value, _data->max_slope);
    }

    float CCharacterController::jump_speed() const
    {
        return _data->jump_speed;
    }

    void CCharacterController::jump_speed(float value)
    {
        checked_setter(value, _data->jump_speed);
    }

    float CCharacterController::fall_speed() const
    {
        return _data->fall_speed;
    }

    void CCharacterController::fall_speed(float value)
    {
        checked_setter(value, _data->fall_speed);
    }

    void CCharacterController::jump() const
    {
        if (on_ground())
        {
            _jump_tags.add_single_tag(entity(), FJumpEvent{});
        }
    }

    void CCharacterController::walk(const Vec2& direction) const
    {
        if (on_ground())
        {
            _walk_tags.add_tag(entity(), FWalkEvent{ direction });
        }
    }

    void CCharacterController::turn(Angle amount) const
    {
        _turn_tags.add_tag(entity(), FTurnEvent{ amount });
    }

    void CCharacterController::generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags)
    {
        // Call the base implementation
        ComponentInterface::generate_tags(tags);

        // Create tags
        _jump_tags.create_buffer(type_info, tags);
        _walk_tags.create_buffer(type_info, tags);
        _turn_tags.create_buffer(type_info, tags);
    }
}
