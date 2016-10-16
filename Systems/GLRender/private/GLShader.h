// GLShader.h
#pragma once

#include <Engine/Resources/Shader.h>
#include "glew.h"

namespace sge
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
