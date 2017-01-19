// GLTexture2D.cpp

#include "../private/GLTexture2D.h"

namespace sge
{
	namespace gl_render
	{
		GLTexture2D::GLTexture2D(const Texture& texture)
		{
			// Create and bind the buffer
			glGenTextures(1, &_id);
			glBindTexture(GL_TEXTURE_2D, _id);

			// Set wrapping parameters to repeat
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// Set sampling parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 8);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.f);

            // Figure out colorspace
            GLenum color_space;
            switch (texture.color_space())
            {
            case Texture::ColorSpace::RGB:
                color_space = GL_RGBA8;
                break;

            case Texture::ColorSpace::S_RGB:
            default:
                color_space = GL_SRGB8_ALPHA8;
                break;
            };

			// Load the image
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
                color_space,
				texture.image.get_width(),
				texture.image.get_height(),
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				texture.image.get_bitmap());

            // Generate mipmaps
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		GLTexture2D::GLTexture2D(GLTexture2D&& move)
			: _id(move._id)
		{
			move._id = 0;
		}

		GLTexture2D::~GLTexture2D()
		{
			glDeleteTextures(1, &_id);
		}
	}
}
