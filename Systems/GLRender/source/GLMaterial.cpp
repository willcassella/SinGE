// GLMaterial.cpp

#include <iostream>
#include <Core/Memory/Functions.h>
#include "../private/GLMaterial.h"
#include "../private/GLRenderSystemState.h"

namespace sge
{
	GLMaterial::GLMaterial(GLRenderSystem::State& renderState, const Material& mat)
	{
		_id = glCreateProgram();

		auto vShader = renderState.find_shader(mat.vertex_shader()).id();
		auto fShader = renderState.find_shader(mat.pixel_shader()).id();

		glAttachShader(_id, vShader);
		glAttachShader(_id, fShader);
		glBindAttribLocation(_id, POSITION_ATTRIB_LOCATION, POSITION_ATTRIB_NAME);
		glBindAttribLocation(_id, NORMAL_ATTRIB_LOCATION, NORMAL_ATTRIB_NAME);
		glBindAttribLocation(_id, TEXCOORD_ATTRIB_LOCATION, TEXCOORD_ATTRIB_NAME);
		glLinkProgram(_id);

		// Make sure program successfully linked
		GLint linked;
		glGetProgramiv(_id, GL_LINK_STATUS, &linked);
		if (linked != GL_TRUE)
		{
			GLsizei length;
			glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &length);

			auto* log = SGE_STACK_ALLOC(GLchar, length);
			glGetProgramInfoLog(_id, length, &length, log);
			std::cerr << "GLRenderSystem: Material compilation failed - '" << log << "'" << std::endl;
		}

		_model = glGetUniformLocation(_id, "model");
		_view = glGetUniformLocation(_id, "view");
		_projection = glGetUniformLocation(_id, "projection");

		glDetachShader(_id, vShader);
		glDetachShader(_id, fShader);

		// Set default parameters
		for (const auto& param : mat.param_table().texture_params)
		{
			GLint location = glGetUniformLocation(_id, param.first.c_str());
			auto tex = renderState.find_texture_2d(param.second).id();
			_texture_params[location] = tex;
		}
	}

	GLMaterial::GLMaterial(GLMaterial&& move)
		: _id(move._id),
		_model(move._model),
		_view(move._view),
		_projection(move._projection)
	{
		move._id = 0;
	}

	GLMaterial::~GLMaterial()
	{
		glDeleteProgram(_id);
	}

	void GLMaterial::bind(GLuint& texIndex) const
	{
		// Activate the program
		glUseProgram(_id);

		// Upload all parameters
		for (const auto& param : _float_params)
		{
			glUniform1f(param.first, param.second);
		}

		for (const auto& param : _vec2_params)
		{
			glUniform1fv(param.first, 2, param.second.vec());
		}

		for (const auto& param : _vec3_params)
		{
			glUniform1fv(param.first, 3, param.second.vec());
		}

		for (const auto& param : _vec4_params)
		{
			glUniform1fv(param.first, 4, param.second.vec());
		}

		for (const auto& param : _texture_params)
		{
			glActiveTexture(texIndex);
			glBindTexture(GL_TEXTURE_2D, param.second);
			glUniform1i(param.first, texIndex);
			texIndex += 1;
		}
	}

	void GLMaterial::override_params(GLRenderSystem::State& renderState, const Material::ParamTable& params, GLuint& texIndex) const
	{
		auto get_uniform_location = [id = this->_id](const std::string& name) {
			// TODO: Error handling
			return glGetUniformLocation(id, name.c_str());
		};

		// Upload all params
		for (const auto& param : params.float_params)
		{
			auto location = get_uniform_location(param.first);
			glUniform1f(location, param.second);
		}

		for (const auto& param : params.vec2_params)
		{
			auto location = get_uniform_location(param.first);
			glUniform1fv(location, 2, param.second.vec());
		}

		for (const auto& param : params.vec3_params)
		{
			auto location = get_uniform_location(param.first);
			glUniform1fv(location, 3, param.second.vec());
		}

		for (const auto& param : params.vec4_params)
		{
			auto location = get_uniform_location(param.first);
			glUniform1fv(location, 4, param.second.vec());
		}

		for (const auto& param : params.texture_params)
		{
			auto texture = renderState.find_texture_2d(param.first).id();
			glActiveTexture(texIndex);
			glBindTexture(GL_TEXTURE_2D, texture);

			auto location = get_uniform_location(param.first);
			glUniform1i(location, texIndex);
			texIndex += 1;
		}
	}
}
