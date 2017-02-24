// GLTexture2D.h
#pragma once

#include <Resource/Resources/Texture.h>
#include "glew.h"

namespace sge
{
	namespace gl_render
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
				return _id;
			}

			//////////////////
			///   Fields   ///
		private:

			Id _id;
		};
	}
}
