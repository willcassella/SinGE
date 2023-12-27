#pragma once

#include <unordered_map>
#include <unordered_set>

#include "lib/base/reflection/reflection.h"

namespace sge
{
    class ArchiveReader;

    struct InputBindings
    {
        SGE_REFLECTED_TYPE;

        void from_archive(ArchiveReader& reader);

        std::unordered_map<int, std::string> key_bindings;
        std::unordered_map<int, std::string> mouse_button_bindings;
        std::unordered_set<std::string> mouse_x_bindings;
        std::unordered_set<std::string> mouse_y_bindings;
        std::unordered_set<std::string> mouse_delta_x_bindings;
        std::unordered_set<std::string> mouse_delta_y_bindings;
    };
}
