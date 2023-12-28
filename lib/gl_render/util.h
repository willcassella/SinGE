#pragma once

#include "lib/gl_render/glew.h"

namespace sge {
namespace gl_render {
enum class GLDebugOutputMode {
  /**
   * \brief No output should be printed to the console.
   */
  NONE,

  /**
   * \brief Output should only be printed to the console in the case of an error.
   */
  ONLY_ERROR,

  /**
   * \brief Any output should be printed to the console.
   */
  ANYTHING,
};

bool debug_shader_status(GLuint shader, GLDebugOutputMode out_mode);

bool debug_program_status(GLuint program, GLDebugOutputMode out_mode);
}  // namespace gl_render
}  // namespace sge
