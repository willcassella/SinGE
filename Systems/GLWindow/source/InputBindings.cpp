// InputBindings.cpp

#include <iostream>
#include <GLFW/glfw3.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Interfaces/IFromArchive.h>
#include "../include/GLWindow/InputBindings.h"

SGE_REFLECT_TYPE(sge::InputBindings)
.implements<IFromArchive>();

namespace sge
{
    void InputBindings::from_archive(ArchiveReader& reader)
    {
        this->bindings.clear();

        reader.enumerate_object_members([this, &reader](const char* event_name)
        {
            std::string key_name;
            sge::from_archive(key_name, reader);
            int key = 0;

            // If it's a single-character name, map the key to the name
            if (key_name.length() == 1)
            {
                key = int(key_name[0]);
            }
            else if (key_name == "SPACE")
            {
                key = GLFW_KEY_SPACE;
            }
            else
            {
                std::cout << "WARNING: Invalid key identifier: '" << key_name << "'" << std::endl;
                return;
            }

            this->bindings[key] = event_name;
        });
    }
}
