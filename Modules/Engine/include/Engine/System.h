// System.h
#pragma once

#include "TagCallback.h"

namespace sge
{
    struct System
    {
        using SystemFn = void(SystemFrame& frame, float current_time, float dt);

        //////////////////
        ///   Fields   ///
    public:

        UFunction<SystemFn> function;

        std::vector<TagCallback> tag_callbacks;
    };
}
