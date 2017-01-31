// Tag.h
#pragma once

#include "UpdatePipeline.h"

namespace sge
{
    struct SystemFrame;

    struct TagCallback
    {
        //////////////////
        ///   Fields   ///
    public:

        /**
         * \brief The type of component this callback is supposed to be called for.
         */
        const TypeInfo* component_type = nullptr;

        /**
         * \brief The entity this callback is supposed to be called on.
         */
        EntityId entity = NULL_ENTITY;

        UpdatePipeline::SystemToken system;

        /**
         * \brief The tag callback.
         */
        UFunction<UpdatePipeline::TagCallbackFn> callback;
    };
}
