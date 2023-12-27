#include <fstream>
#include <iostream>

#include "lib/gl_render/gl_shader.h"
#include "lib/gl_render/util.h"

namespace sge
{
    namespace gl_render
    {
        GLuint create_shader(
            GLenum type,
            const char* source)
        {
            const auto id = glCreateShader(type);
            glShaderSource(id, 1, &source, nullptr);
            glCompileShader(id);

            // Make sure the shader compiled succesfully
            debug_shader_status(id, GLDebugOutputMode::ONLY_ERROR);
            return id;
        }

        GLuint load_shader(
            GLenum type,
            const char* path)
        {
            // Open a file containing the shader source code
            std::ifstream file{ path };

            // Figure out the size of the file
            file.seekg(0, std::ios::end);
            std::size_t len = static_cast<std::size_t>(file.tellg());
            file.seekg(0);

            // Allocate a string for the source code
            std::string source(len, ' ');
            file.read(&source[0], len);

            // Compile the shader
            return create_shader(type, source.c_str());
        }

        void free_shader(
            GLuint id)
        {
            glDeleteShader(id);
        }
    }
}
