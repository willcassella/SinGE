#include <iostream>

#include "lib/base/memory/functions.h"
#include "lib/gl_render/util.h"

namespace sge {
namespace gl_render {
bool debug_shader_status(GLuint shader, GLDebugOutputMode out_mode) {
  GLint compiled = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

  // If the shader failed to compile and the user did not ask to suppress output, or the user wants output
  // anyway
  if ((!compiled && out_mode != GLDebugOutputMode::NONE) || out_mode == GLDebugOutputMode::ANYTHING) {
    GLsizei log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

    auto* log = SGE_STACK_ALLOC(GLchar, log_length);
    glGetShaderInfoLog(shader, log_length, &log_length, log);
    std::cout << "GLRenderSystem: shader status - '" << log << "'" << std::endl;
  }

  return compiled == GL_TRUE;
}

bool debug_program_status(GLuint program, GLDebugOutputMode out_mode) {
  GLint linked = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);

  // If the program failed to link and the user did not ask to suppress output, or the user wants output
  // anyway
  if ((!linked && out_mode != GLDebugOutputMode::NONE) || out_mode == GLDebugOutputMode::ANYTHING) {
    // Get the log length
    GLsizei log_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

    auto* log = SGE_STACK_ALLOC(GLchar, log_length);
    glGetProgramInfoLog(program, log_length, &log_length, log);
    std::cout << "GLRenderSystem: material status - '" << log << "'" << std::endl;
  }

  return linked == GL_TRUE;
}
}  // namespace gl_render
}  // namespace sge
