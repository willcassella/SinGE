// CSensor.h
#pragma once

#include "../../Component.h"

namespace sge
{
    class SGE_ENGINE_API CSensor final : public TComponentInterface<CSensor>
    {
    public:

        SGE_REFLECTED_TYPE;

        ////////////////////////
        ///   Constructors   ///
    public:

        CSensor(ProcessingFrame& pframe, EntityId entity);

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);
    };
}
