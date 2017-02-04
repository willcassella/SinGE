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
         * \brief The token uniquely identifying this system.
         */
        UpdatePipeline::SystemToken system_token = UpdatePipeline::NO_SYSTEM;

        /**
         * \brief Async token, identifies which systems must be run synchronously together.
         */
        UpdatePipeline::AsyncToken async_token = UpdatePipeline::FULLY_ASYNC;

        /**
         * \brief Actual system function to run.
         */
        UFunction<UpdatePipeline::SystemFn> system_fn;
    };
}
