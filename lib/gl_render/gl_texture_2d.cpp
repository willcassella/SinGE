#include "lib/gl_render/gl_texture_2d.h"

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
            GLenum upload_type)
        {
            // Create and bind the buffer
            GLuint id = 0;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            // Set wrapping parameters to repeat
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // Set sampling parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 8);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.f);

            // Load the image
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                internal_format,
                width,
                height,
                0,
                upload_format,
                upload_type,
                data);

            // Generate mipmaps
            glGenerateMipmap(GL_TEXTURE_2D);

            return id;
        }
    }
}
