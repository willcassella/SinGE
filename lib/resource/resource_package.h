#pragma once

#include <unordered_map>

#include "lib/base/containers/fixed_string.h"
#include "lib/base/reflection/reflection.h"
#include "lib/resource/build.h"

namespace sge
{
    struct SGE_RESOURCE_API ResourcePackage
    {
        SGE_REFLECTED_TYPE;
        using PackageName = FixedString<8>;
        using ResourceName = FixedString<32>;

    private:
        std::unordered_map<ResourceName, void*> _resources;
    };
}
