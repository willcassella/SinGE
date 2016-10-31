// GLShader.cpp - Copyright 2013-2016 Will Cassella, All Rights Reserved

#include <fstream>
#include <iostream>
#include <Core/Memory/Functions.h>
#include "../private/GLShader.h"

namespace sge
{
	static void compile_shader(GLuint id, const char* source)
	{
		glShaderSource(id, 1, &source, nullptr);
		glCompileShader(id);

		// Make sure the shader compiled successfully
		GLint compiled;
		glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
		if (compiled != GL_TRUE)
		{
			GLsizei length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

			auto* log = SGE_STACK_ALLOC(GLchar, length);
			glGetShaderInfoLog(id, length, &length, log);
			std::cerr << "GLRenderSystem: Shader compilation failed - '" << log << "'" << std::endl;
		}
	}

	GLShader::GLShader(GLenum type, const std::string& path)
	{
		_id = glCreateShader(type);

		// Open a file containing the shader source code
		std::ifstream file{ path };

		// Figure out the size of the file
		file.seekg(0, std::ios::end);
		std::size_t len = static_cast<std::size_t>(file.tellg());
		file.seekg(0);

		// Allocate a string for the source code
		std::string source(len, ' ');
		file.read(&source[0], len);

		// Compile the shader
		compile_shader(_id, source.c_str());
	}

	GLShader::GLShader(const VertexShader& shader)
	{
		_id = glCreateShader(GL_VERTEX_SHADER);
		compile_shader(_id, shader.source().c_str());
	}

	GLShader::GLShader(const PixelShader& shader)
	{
		_id = glCreateShader(GL_FRAGMENT_SHADER);
		compile_shader(_id, shader.source().c_str());
	}

	GLShader::GLShader(GLShader&& move)
		: _id(move._id)
	{
		move._id = 0;
	}

	GLShader::~GLShader()
	{
		glDeleteShader(_id);
	}
}
