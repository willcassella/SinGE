// CCharacterController.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Gameplay/CCharacterController.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"

SGE_REFLECT_TYPE(sge::CCharacterController)
.property("step_height", &CCharacterController::step_height, &CCharacterController::step_height)
.property("max_slope", &CCharacterController::max_slope, &CCharacterController::max_slope)
.property("jump_speed", &CCharacterController::jump_speed, &CCharacterController::jump_speed)
.property("fall_speed", &CCharacterController::fall_speed, &CCharacterController::fall_speed)
.property("on_ground", &CCharacterController::on_ground, nullptr, PF_EDITOR_HIDDEN);

namespace sge
{
    struct CCharacterController::SharedData : CSharedData<CCharacterController>
    {
		////////////////////////
		///   Constructors   ///
    public:

		SharedData()
			: jump_channel(sizeof(EJump), 2),
			turn_channel(sizeof(ETurn), 2),
			walk_channel(sizeof(EWalk), 2)
    	{
		}

		///////////////////
		///   Methods   ///
    public:

		void reset()
		{
			CSharedData<CCharacterController>::reset();
			jump_channel.clear();
			turn_channel.clear();
			walk_channel.clear();
		}

		void on_end_update_frame()
		{
			CSharedData<CCharacterController>::on_end_update_frame();
			jump_channel.clear();
			turn_channel.clear();
			walk_channel.clear();
		}

		EventChannel* get_event_channel(const char* channel_name)
		{
			if (std::strcmp(channel_name, "jump_channel") == 0)
			{
				return &jump_channel;
			}
			else if (std::strcmp(channel_name, "turn_channel") == 0)
			{
				return &turn_channel;
			}
			else if (std::strcmp(channel_name, "walk_channel") == 0)
			{
				return &walk_channel;
			}

			return CSharedData<CCharacterController>::get_event_channel(channel_name);
		}

		//////////////////
		///   Fields   ///
    public:

		EventChannel jump_channel;
		EventChannel turn_channel;
		EventChannel walk_channel;
    };

	CCharacterController::CCharacterController(NodeId node, SharedData& shared_data)
		: _node(node),
		_shared_data(&shared_data)
	{
	}

    void CCharacterController::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CCharacterController, SharedData>>());
    }

	void CCharacterController::to_archive(ArchiveWriter& writer) const
	{
		writer.object_member("stpht", _step_height);
		writer.object_member("mxslp", _max_slope);
		writer.object_member("jspd", _jump_speed);
		writer.object_member("fspd", _fall_speed);
		writer.object_member("grd", _on_ground);
	}

	void CCharacterController::from_archive(ArchiveReader& reader)
	{
		reader.object_member("stpht", _step_height);
		reader.object_member("mxslp", _max_slope);
		reader.object_member("jspd", _jump_speed);
		reader.object_member("fsdp", _fall_speed);
		reader.object_member("grd", _on_ground);
	}

	NodeId CCharacterController::node() const
	{
		return _node;
	}

    bool CCharacterController::on_ground() const
    {
        return _on_ground;
    }

    void CCharacterController::set_on_ground(bool value)
    {
        _on_ground = value;
    }

    float CCharacterController::step_height() const
    {
        return _step_height;
    }

    void CCharacterController::step_height(float value)
    {
		if (_step_height != value)
		{
			_step_height = value;
			set_modified("step_height");
		}
    }

    Angle CCharacterController::max_slope() const
    {
        return _max_slope;
    }

    void CCharacterController::max_slope(Angle value)
    {
		if (_max_slope != value)
		{
			_max_slope = value;
			set_modified("max_slope");
		}
    }

    float CCharacterController::jump_speed() const
    {
        return _jump_speed;
    }

    void CCharacterController::jump_speed(float value)
    {
		if (_jump_speed != value)
		{
			_jump_speed = value;
			set_modified("jump_speed");
		}
    }

    float CCharacterController::fall_speed() const
    {
		return _fall_speed;
    }

    void CCharacterController::fall_speed(float value)
    {
		if (_fall_speed != value)
		{
			_fall_speed = value;
			set_modified("fall_speed");
		}
    }

    void CCharacterController::jump() const
    {
        if (on_ground())
        {
			EJump event;
			event.node = _node;
			_shared_data->jump_channel.append(&event, sizeof(EJump), 1);
        }
    }

    void CCharacterController::walk(const Vec2& direction) const
    {
        if (on_ground())
        {
			EWalk event;
			event.node = _node;
			event.direction = direction;
			_shared_data->walk_channel.append(&event, sizeof(EWalk), 1);
        }
    }

    void CCharacterController::turn(Angle amount) const
    {
		ETurn event;
		event.node = _node;
		event.amount = amount;
		_shared_data->turn_channel.append(&event, sizeof(ETurn), 1);
    }

	void CCharacterController::set_modified(const char* property_name)
	{
		_shared_data->set_modified(_node, this, property_name);
	}
}
