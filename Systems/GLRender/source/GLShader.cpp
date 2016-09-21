// GLShader.cpp - Copyright 2013-2016 Will Cassella, All Rights Reserved

#include <iostream>
#include "../private/glew.h"
#include "../include/GLRender/GLShader.h"

namespace sge
{
	GLShader::GLShader(const Shader& shader)
	{
		// Identify the shader type and construct the shader
		switch (shader.get_shader_type())
		{
		case Shader::Type::VERTEX_SHADER:
			this->_id = glCreateShader(GL_VERTEX_SHADER);
			break;

		case Shader::Type::PIXEL_SHADER:
			this->_id = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		}

		// Compile the shader
		CString tempSource = shader.get_source().c_str();
		glShaderSource(this->_id, 1, &tempSource, nullptr);
		glCompileShader(this->_id);

		// Make sure the shader compiled successfully
		GLint compiled;
		glGetShaderiv(this->_id, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLsizei length;
			glGetShaderiv(this->_id, GL_INFO_LOG_LENGTH, &length);

			GLchar* log = new GLchar[length + 1];
			glGetShaderInfoLog(this->_id, length, &length, log);
			std::cout << "Shader compilation failed: " << log << std::endl;
			delete[] log;
		}
	}

	GLShader::GLShader(const std::string& path)
		// TODO
	{
	}

	GLShader::GLShader(GLShader&& move)
	{
		this->_id = move._id;
		move._id = 0;
	}

	GLShader::~GLShader()
	{
		glDeleteShader(this->_id);
	}
}
