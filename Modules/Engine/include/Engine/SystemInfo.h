// SystemInfo.h
#pragma once

#include "UpdatePipeline.h"

namespace sge
{
    struct SystemInfo
    {
        //////////////////
        ///   Fields   ///
    public:

        /**
         * \brief The name of this system.
         */
        std::string name;

        /**
         * \brief Actual system function to run.
         */
        UFunction<UpdatePipeline::SystemFn> system_fn;
    };
}
