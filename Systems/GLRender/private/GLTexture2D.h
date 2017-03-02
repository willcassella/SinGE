// GLTexture2D.h
#pragma once

#include <Core/env.h>
#include "glew.h"

namespace sge
{
	namespace gl_render
	{
        GLuint create_texture(
            int32 width,
            int32 height,
            const void* data,
            GLenum internal_format,
            GLenum upload_format,
            GLenum upload_type);
	}
}
