// ResourceInfo.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "build.h"

namespace sge
{
    struct SGE_RESOURCE_API ResourceInfo
    {
        SGE_REFLECTED_TYPE;

        //////////////////
        ///   Fields   ///
    public:

        std::string type_name;

    };
}
