// CLightMaskReceiver.h
#pragma once

#include "../../Component.h"

namespace sge
{
    class SGE_ENGINE_API CLightMaskReceiver final : public TComponentInterface<CLightMaskReceiver>
    {
    public:

        SGE_REFLECTED_TYPE;

        ////////////////////////
        ///   Constructors   ///
    public:

        CLightMaskReceiver(ProcessingFrame& pframe, EntityId entity);

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);
    };
}
