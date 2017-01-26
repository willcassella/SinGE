// CLightMaskObstructor.h
#pragma once

#include "../../Component.h"

namespace sge
{
    class SGE_ENGINE_API CLightMaskObstructor final : public TComponentInterface<CLightMaskObstructor>
    {
    public:

        SGE_REFLECTED_TYPE;

        ////////////////////////
        ///   Constructors   ///
    public:

        CLightMaskObstructor(ProcessingFrame& pframe, EntityId entity);

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);
    };
}
