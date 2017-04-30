// ChangeLevelSystem.cpp

#include "../../include/Engine/Systems/ChangeLevelSystem.h"
#include "../../include/Engine/Components/Gameplay/CLevelPortal.h"
#include "../../include/Engine/Scene.h"
#include "../../include/Engine/UpdatePipeline.h"

namespace sge
{
	void ChangeLevelSystem::pipeline_register(UpdatePipeline& pipeline)
	{
		pipeline.register_system_fn("check_change_level", this, &ChangeLevelSystem::check_change_level);
	}

	void ChangeLevelSystem::initialize_subscriptions(Scene& scene)
	{
		_change_level_channel = scene.get_event_channel(CLevelPortal::type_info, "change_level");
		_change_level_sid = _change_level_channel->subscribe();
	}

	bool ChangeLevelSystem::requires_change_level() const
	{
		return !_change_level_target.empty();
	}

	const std::string& ChangeLevelSystem::change_level_target() const
	{
		return _change_level_target;
	}

	void ChangeLevelSystem::check_change_level(Scene& /*scene*/, SystemFrame& /*frame*/)
	{
		CLevelPortal::EChangeLevel event;
		if (_change_level_target.empty() && _change_level_channel->consume(_change_level_sid, 1, &event, nullptr))
		{
			_change_level_target = event.target_path.c_str();
		}
	}
}
