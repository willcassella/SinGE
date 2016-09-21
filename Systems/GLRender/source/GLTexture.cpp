// GLTexture.cpp - Copyright 2013-2016 Will Cassella, All Rights Reserved

#include "glew.h"
#include "../include/GLRender/GLTexture.h"

namespace willow
{
	////////////////////////
	///   Constructors   ///

	GLTexture::GLTexture(const Texture& image)
	{
		// Create and bind the buffer
		glGenTextures(1, &this->_id);
		glBindTexture(GL_TEXTURE_2D, this->_id);

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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.get_width(), image.get_height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.get_bitmap());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	GLTexture::GLTexture(GLTexture&& move)
	{
		this->_id = move._id;
		move._id = 0;
	}

	GLTexture::~GLTexture()
	{
		glDeleteTextures(1, &this->_id);
	}
}
