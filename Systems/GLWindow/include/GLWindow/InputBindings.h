// Input Bindings.h
#pragma once

#include <unordered_set>
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

        std::unordered_map<int, std::string> key_bindings;
        std::unordered_map<int, std::string> mouse_button_bindings;
        std::unordered_set<std::string> mouse_x_bindings;
        std::unordered_set<std::string> mouse_y_bindings;
    };
}
