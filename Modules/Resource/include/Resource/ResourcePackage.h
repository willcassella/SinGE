// ResourcePackage.h
#pragma once

#include <unordered_map>
#include <Core/Reflection/Reflection.h>
#include <Core/Containers/FixedString.h>
#include "build.h"

namespace sge
{
    struct SGE_RESOURCE_API ResourcePackage
    {
        SGE_REFLECTED_TYPE;
        using PackageName = FixedString<8>;
        using ResourceName = FixedString<32>;

        //////////////////
        ///   Fields   ///
    private:

        std::unordered_map<ResourceName, void*> _resources;
    };
}
