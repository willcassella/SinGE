#pragma once

#include "lib/gl_render/glew.h"
#include "lib/resource/resources/shader.h"

namespace sge {
namespace gl_render {
GLuint load_shader(GLuint type, const char* path);

GLuint create_shader(GLenum type, const char* source);

void free_shader(GLuint id);
}  // namespace gl_render
}  // namespace sge
