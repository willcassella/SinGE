// Tag.h
#pragma once

#include <Core/Functional/UFunction.h>
#include "Component.h"

namespace sge
{
    struct SystemFrame;

    struct TagCallback
    {
        using CallbackFn = void(SystemFrame& frame, Any<> tag, ComponentId component);

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

        /**
         * \brief The tag callback.
         */
        UFunction<CallbackFn> callback;
    };
}
