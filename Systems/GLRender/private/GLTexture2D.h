// GLTexture2D.h
#pragma once

#include <Engine/Resources/Texture.h>
#include "glew.h"

namespace sge
{
	struct GLTexture2D
	{
		using Id = GLuint;

		////////////////////////
		///   Constructors   ///
	public:

		GLTexture2D(const Texture& texture);

		GLTexture2D(GLTexture2D&& move);
		~GLTexture2D();

		///////////////////
		///   Methods   ///
	public:

		Id id() const
		{
			return this->_id;
		}

		//////////////////
		///   Fields   ///
	private:

		Id _id;
	};
}
