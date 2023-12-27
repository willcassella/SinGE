#include <algorithm>

#include "lib/engine/components/gameplay/level_portal.h"
#include "lib/engine/scene.h"
#include "lib/engine/system_frame.h"
#include "lib/engine/systems/change_level_system.h"
#include "lib/engine/update_pipeline.h"

namespace sge
{
    void ChangeLevelSystem::pipeline_register(UpdatePipeline& pipeline)
    {
        pipeline.register_system_fn("check_change_level", this, &ChangeLevelSystem::update);
    }

    void ChangeLevelSystem::initialize_subscriptions(Scene& scene)
    {
        _change_level_channel = scene.get_event_channel(CLevelPortal::type_info, "change_level");
        _change_level_sid = _change_level_channel->subscribe();
    }

    bool ChangeLevelSystem::requires_change_level() const
    {
        return !_change_level_target.empty() && _change_level_timer > _change_level_timer_end;
    }

    const std::string& ChangeLevelSystem::change_level_target() const
    {
        return _change_level_target;
    }

    void ChangeLevelSystem::reset()
    {
        _change_level_target.clear();
        _change_level_timer = 0.f;
        _init_gamma = _target_gamma;
        _target_gamma = 2.2f;
        _init_brightness_boost = _target_brightness_boost;
        _target_brightness_boost = 0.f;
    }

    void ChangeLevelSystem::update(Scene& scene, SystemFrame& frame)
    {
        _change_level_timer += frame.time_delta();

        // Interpolate gamma and brightness
        const float v = std::min(1.f, _change_level_timer / _change_level_timer_end);
        scene.get_raw_scene_data().scene_gamma = _init_gamma + v * (_target_gamma - _init_gamma);
        scene.get_raw_scene_data().scene_brightness_boost = _init_brightness_boost + v * (_target_brightness_boost - _init_brightness_boost);

        // Check for a change level event
        CLevelPortal::EChangeLevel event;
        if (_change_level_target.empty() && _change_level_channel->consume(_change_level_sid, 1, &event, nullptr))
        {
            _change_level_target = event.component->level_path();
            _change_level_timer = 0;
            _change_level_timer_end = event.component->fade_duration();
            if (event.component->gamma_fade())
            {
                _init_gamma = 2.2f;
                _target_gamma = 0.f;
                _init_brightness_boost = 0.f;
                _target_brightness_boost = 0.f;
            }
            else if (event.component->brightness_fade())
            {
                _init_gamma = 2.2f;
                _target_gamma = 2.2f;
                _init_brightness_boost = 0.f;
                _target_brightness_boost = 1.f;
            }
        }
    }
}
