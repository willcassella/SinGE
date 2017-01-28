// InputBindings.cpp

#include <iostream>
#include <GLFW/glfw3.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Interfaces/IFromArchive.h>
#include "../include/GLWindow/InputBindings.h"

SGE_REFLECT_TYPE(sge::InputBindings)
.implements<IFromArchive>();

static constexpr const char MOUSE_PREFIX[] = "MOUSE_";
static constexpr auto MOUSE_PREFIX_LEN = sizeof(MOUSE_PREFIX) - 1;

namespace sge
{
    void InputBindings::from_archive(ArchiveReader& reader)
    {
        key_bindings.clear();
        mouse_button_bindings.clear();

        reader.enumerate_object_members([this, &reader](const char* event_name)
        {
            std::string input_name;
            sge::from_archive(input_name, reader);
            int key = 0;

            // If the input name is a mouse input
            if (std::strncmp(input_name.c_str(), MOUSE_PREFIX, MOUSE_PREFIX_LEN) == 0)
            {
                const char* mouse_input_name = input_name.c_str() + MOUSE_PREFIX_LEN;

                if (std::strcmp(mouse_input_name, "X") == 0)
                {
                    this->mouse_x_bindings.insert(event_name);
                }
                else if (std::strcmp(mouse_input_name, "Y") == 0)
                {
                    this->mouse_y_bindings.insert(event_name);
                }
                else if (std::strcmp(mouse_input_name, "L") == 0)
                {
                    this->mouse_button_bindings[GLFW_MOUSE_BUTTON_LEFT] = event_name;
                }
                else if (std::strcmp(mouse_input_name, "R") == 0)
                {
                    this->mouse_button_bindings[GLFW_MOUSE_BUTTON_RIGHT] = event_name;
                }
                else if (std::strcmp(mouse_input_name, "M") == 0)
                {
                    this->mouse_button_bindings[GLFW_MOUSE_BUTTON_MIDDLE] = event_name;
                }
                else
                {
                    std::cout << "WARNING: Invalid mouse button identifier: '" << input_name << "'" << std::endl;
                }

                return;
            }
            else if (input_name == "SPACE")
            {
                key = GLFW_KEY_SPACE;
            }
            else if (input_name.length() == 1)
            {
                // If it's a single-character name, use dierect mappping from the name to the key
                key = int(input_name[0]);
            }
            else
            {
                std::cout << "WARNING: Invalid key identifier: '" << input_name << "'" << std::endl;
                return;
            }

            this->key_bindings[key] = event_name;
        });
    }
}
