// GLShader.h
#pragma once

#include <Resource/Resources/Shader.h>
#include "glew.h"

namespace sge
{
	namespace gl_render
	{
		struct GLShader
		{
			using Id = GLuint;

			////////////////////////
			///   Constructors   ///
		public:

			GLShader(GLenum type, const std::string& path);

			GLShader(const VertexShader& shader);

			GLShader(const PixelShader& shader);

			GLShader(GLShader&& move);
			~GLShader();

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
