#pragma once

#include "lib/engine/system_frame.h"

namespace sge
{
    struct SGE_ENGINE_API AnimationSystem
    {
        void register_pipeline(UpdatePipeline& pipeline);

    private:
        void animation_update(Scene& scene, SystemFrame& frame);

        void animation_apply(Scene& scene, SystemFrame& frame);
    };
}
