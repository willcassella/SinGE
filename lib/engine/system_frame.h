#pragma once

#include <stack>
#include <stdint.h>

#include "lib/base/functional/ufunction.h"
#include "lib/engine/update_pipeline.h"

namespace sge
{
    struct ProcessingFrame;
    struct SceneData;
    struct Scene;

    struct SGE_ENGINE_API SystemFrame
    {
        SGE_REFLECTED_TYPE;

        void yield();

        uint64_t frame_id() const;

        float current_time() const;

        float time_delta() const;

        void push(const char* system_name);

    private:
        /* Only 'Scene' objects may construct SystemFrames. */
        friend Scene;

        SystemFrame() = default;
        SystemFrame(SystemFrame&& move) = delete;
        SystemFrame(const SystemFrame& copy) = delete;
        SystemFrame& operator=(const SystemFrame& copy) = delete;
        SystemFrame& operator=(SystemFrame&& move) = delete;

        float _current_time = 0.f;
        float _time_delta = 0.f;
        Scene* _scene = nullptr;
        UpdatePipeline* _update_pipeline = nullptr;
        std::vector<SystemInfo*> _job_queue;
    };
}
