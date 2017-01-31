// CLightMaskReceiver.h
#pragma once

#include "../../Component.h"

namespace sge
{
    class SGE_ENGINE_API CLightMaskReceiver final : public TComponentInterface<CLightMaskReceiver>
    {
    public:

        SGE_REFLECTED_TYPE;

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);

        void reset();
    };
}
