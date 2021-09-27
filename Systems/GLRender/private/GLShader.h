// GLShader.h
#pragma once

#include <Resource/Resources/Shader.h>
#include "glew.h"

namespace sge
{
    namespace gl_render
    {
        GLuint load_shader(
            GLuint type,
            const char* path);

        GLuint create_shader(
            GLenum type,
            const char* source);

        void free_shader(
            GLuint id);
    }
}
