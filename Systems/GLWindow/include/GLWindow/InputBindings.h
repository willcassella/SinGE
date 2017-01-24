// Input Bindings.h
#pragma once

#include <unordered_map>
#include <Core/Reflection/Reflection.h>

namespace sge
{
    class ArchiveReader;

    struct InputBindings
    {
        SGE_REFLECTED_TYPE;

        ///////////////////
        ///   Methods   ///
    public:

        void from_archive(ArchiveReader& reader);

        //////////////////
        ///   Fields   ///
    public:

        std::unordered_map<int, std::string> bindings;
    };
}
