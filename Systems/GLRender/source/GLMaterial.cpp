// GLMaterial.cpp - Copyright 2013-2016 Will Cassella, All Rights Reserved

#include <Core/IO/Console.h>
#include "glew.h"
#include "../include/GLRender/GLRenderSystem.h"

namespace willow
{
	////////////////////////
	///   Constructors   ///

	GLMaterial::GLMaterial(GLRenderSystem& renderer, const Material& mat)
		: _params(mat.default_params)
	{
		_id = glCreateProgram();

		BufferID vShader = renderer.find_shader(mat.vertex_shader).get_id();
		BufferID fShader = renderer.find_shader(mat.fragment_shader).get_id();

		glAttachShader(_id, vShader);
		glAttachShader(_id, fShader);
		glBindAttribLocation(_id, 0, "vPosition");
		glBindAttribLocation(_id, 1, "vTexCoord");
		glBindAttribLocation(_id, 2, "vNormal");
		glLinkProgram(_id);

		// Make sure program successfully linked
		GLint linked;
		glGetProgramiv(_id, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			GLsizei length;
			glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &length);

			GLchar* log = new GLchar[length + 1];
			glGetProgramInfoLog(_id, length, &length, log);
			Console::WriteLine("Material compilation failed: \"@\"", log);
			delete[] log;
		}

		_model = glGetUniformLocation(_id, "model");
		_view = glGetUniformLocation(_id, "view");
		_projection = glGetUniformLocation(_id, "projection");

		glDetachShader(_id, vShader);
		glDetachShader(_id, fShader);
	}

	GLMaterial::GLMaterial(GLMaterial&& move)
		: _params(std::move(move._params))
	{
		_id = move._id;
		_model = move._model;
		_view = move._view;
		_projection = move._projection;

		move._id = 0;
	}

	GLMaterial::~GLMaterial()
	{
		glDeleteProgram(_id);
	}

	///////////////////
	///   Methods   ///

	void GLMaterial::bind(GLRenderSystem& renderer, const Table<String, Material::Param>& instanceParams)
	{
		uint32 texIndex = 0;
		glUseProgram(_id);
		this->upload_params(renderer, this->_params, texIndex);
		this->upload_params(renderer, instanceParams, texIndex);
	}

	void GLMaterial::upload_params(GLRenderSystem& renderer, const Table<String, Material::Param>& params, uint32& texIndex)
	{
		for (const auto& param : params)
		{
			// Handles parameter binding in a generic way
			auto bindHandler = [&](auto value)
			{
				using T = std::decay_t<decltype(value)>;

				// Get param location
				int32 location = glGetUniformLocation(_id, param.First.Cstr());

				// Handle texture case
				if (std::is_same<ResourceHandle<Texture>, T>::value)
				{
					const auto& texValue = reinterpret_cast<const ResourceHandle<Texture>&>(value);

					// Set active texture, and upload
					glActiveTexture(GL_TEXTURE0 + texIndex);
					glBindTexture(GL_TEXTURE_2D, renderer.find_texture(texValue).get_id());
					glUniform1i(location, texIndex);
					++texIndex;
				}

				// Upload the parameter
				this->upload_param(location, value);
			};

			param.Second.Invoke(bindHandler);
		}
	}

	void GLMaterial::upload_param(int32 location, float value) const
	{
		glUniform1f(location, value);
	}

	void GLMaterial::upload_param(int32 location, Vec2 value) const
	{
		glUniform1fv(location, 2, (const GLfloat*)&value);
	}

	void GLMaterial::upload_param(int32 location, Vec3 value) const
	{
		glUniform1fv(location, 3, (const GLfloat*)&value);
	}

	void GLMaterial::upload_param(int32 location, Vec4 value) const
	{
		glUniform1fv(location, 4, (const GLfloat*)&value);
	}

	void GLMaterial::upload_param(int32 /*location*/, ResourceHandle<Texture> /*value*/)
	{
		// Do nothing, uploading performed in bind handler
	}
}
