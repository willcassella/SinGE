#pragma once

#include "lib/base/reflection/reflection.h"
#include "lib/resource/build.h"

namespace sge
{
    struct SGE_RESOURCE_API ResourceInfo
    {
        SGE_REFLECTED_TYPE;

        std::string type_name;
    };
}
