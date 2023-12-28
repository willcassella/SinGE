#pragma once

#include <stdint.h>

#include "lib/base/env.h"
#include "lib/gl_render/glew.h"

namespace sge
{
    namespace gl_render
    {
        GLuint create_texture(
            int32_t width,
            int32_t height,
            const void* data,
            GLenum internal_format,
            GLenum upload_format,
            GLenum upload_type);
    }
}
