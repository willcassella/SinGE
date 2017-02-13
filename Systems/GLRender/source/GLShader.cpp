// GLShader.cpp - Copyright 2013-2016 Will Cassella, All Rights Reserved

#include <fstream>
#include <iostream>
#include "../private/GLShader.h"
#include "../private/Util.h"

namespace sge
{
	namespace gl_render
	{
		static void compile_shader(GLuint id, const char* source)
		{
			glShaderSource(id, 1, &source, nullptr);
			glCompileShader(id);

			// Make sure the shader compiled succesfully
            debug_shader_status(id, GLDebugOutputMode::ONLY_ERROR);
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
}
