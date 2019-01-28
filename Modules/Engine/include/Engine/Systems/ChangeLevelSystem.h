// ChangeLevelSystem.h
#pragma once

#include "../Component.h"

namespace sge
{
    struct UpdatePipeline;

    struct SGE_ENGINE_API ChangeLevelSystem
    {
        /*--- Methods ---*/
    public:

        void pipeline_register(UpdatePipeline& pipeline);

        void initialize_subscriptions(Scene& scene);

        bool requires_change_level() const;

        const std::string& change_level_target() const;

        void reset();

    private:

        void update(Scene& scene, SystemFrame& frame);

        /*--- Fields ---*/
    private:

        float _change_level_timer_end = 3.f;
        float _change_level_timer = 0.f;
        float _init_gamma = 0.f;
        float _target_gamma = 2.2f;
        float _init_brightness_boost = 0.f;
        float _target_brightness_boost = 0.f;
        std::string _change_level_target;
        EventChannel* _change_level_channel = nullptr;
        EventChannel::SubscriberId _change_level_sid = EventChannel::INVALID_SID;
    };
}
