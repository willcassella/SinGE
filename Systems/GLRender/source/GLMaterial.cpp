// GLMaterial.cpp

#include <iostream>
#include "../private/GLMaterial.h"
#include "../private/GLRenderSystemState.h"

namespace sge
{
	namespace gl_render
	{
		GLMaterial::GLMaterial(GLRenderSystem::State& renderState, const Material& mat)
		{
			_id = glCreateProgram();

			const GLShader::Id v_shader = renderState.get_shader_resource(mat.vertex_shader());
			const GLShader::Id f_shader = renderState.get_shader_resource(mat.pixel_shader());

			glAttachShader(_id, v_shader);
			glAttachShader(_id, f_shader);
			glBindAttribLocation(_id, POSITION_ATTRIB_LOCATION, POSITION_ATTRIB_NAME);
			glBindAttribLocation(_id, NORMAL_ATTRIB_LOCATION, NORMAL_ATTRIB_NAME);
            glBindAttribLocation(_id, TANGENT_ATTRIB_LOCATION, TANGENT_ATTRIB_NAME);
            glBindAttribLocation(_id, BITANGENT_SIGN_ATTRIB_LOCATION, BITANGENT_SIGN_ATTRIB_NAME);
			glBindAttribLocation(_id, TEXCOORD_ATTRIB_LOCATION, TEXCOORD_ATTRIB_NAME);
			glLinkProgram(_id);

            // Make sure the program linked succesfully
            debug_program_status(_id, GLDebugOutputMode::ONLY_ERROR);

			_model = glGetUniformLocation(_id, "model");
			_view = glGetUniformLocation(_id, "view");
			_projection = glGetUniformLocation(_id, "projection");

			glDetachShader(_id, v_shader);
			glDetachShader(_id, f_shader);

            // Set default bool parameters
            for (const auto& param : mat.param_table().bool_params)
            {
                const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
                    _bool_params[location] = param.second;
                }
            }

            // Set default float parameters
            for (const auto& param : mat.param_table().float_params)
            {
                const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
                    _float_params[location] = param.second;
                }
            }

            // Set default vec2 parameters
            for (const auto& param : mat.param_table().vec2_params)
            {
                const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
                    _vec2_params[location] = param.second;
                }
            }

            // Set default vec3 parameters
            for (const auto& param : mat.param_table().vec3_params)
            {
                const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
                    _vec3_params[location] = param.second;
                }
            }

            // Set default vec4 parameters
            for (const auto& param : mat.param_table().vec4_params)
            {
                const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
                    _vec4_params[location] = param.second;
                }
            }

			// Set default texture parameters
			for (const auto& param : mat.param_table().texture_params)
			{
                const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
				    const auto tex = renderState.get_texture_2d_resource(param.second);
				    _texture_params[location] = tex;
                }
			}
		}

		GLMaterial::GLMaterial(GLMaterial&& move)
			: _id(move._id),
			_model(move._model),
			_view(move._view),
			_projection(move._projection),
            _bool_params(std::move(move)._bool_params),
			_float_params(std::move(move)._float_params),
			_vec2_params(std::move(move)._vec2_params),
			_vec3_params(std::move(move)._vec3_params),
			_vec4_params(std::move(move)._vec4_params),
			_texture_params(std::move(move)._texture_params)
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
            for (const auto& param : _bool_params)
            {
                glUniform1i(param.first, param.second ? 1 : 0);
            }

			for (const auto& param : _float_params)
			{
				glUniform1f(param.first, param.second);
			}

			for (const auto& param : _vec2_params)
			{
				glUniform2fv(param.first, 1, param.second.vec());
			}

			for (const auto& param : _vec3_params)
			{
				glUniform3fv(param.first, 1, param.second.vec());
			}

			for (const auto& param : _vec4_params)
			{
				glUniform4fv(param.first, 1, param.second.vec());
			}

			for (const auto& param : _texture_params)
			{
				glActiveTexture(texIndex);
				glBindTexture(GL_TEXTURE_2D, param.second);
				glUniform1i(param.first, texIndex - GL_TEXTURE0);
				texIndex += 1;
			}
		}

		void GLMaterial::set_model_matrix(const Mat4& model) const
		{
			glUniformMatrix4fv(_model, 1, GL_FALSE, model.vec());
		}

		void GLMaterial::set_view_matrix(const Mat4& view) const
		{
			glUniformMatrix4fv(_view, 1, GL_FALSE, view.vec());
		}

		void GLMaterial::set_projection_matrix(const Mat4& projection) const
		{
			glUniformMatrix4fv(_projection, 1, GL_FALSE, projection.vec());
		}

		void GLMaterial::override_params(GLRenderSystem::State& renderState, const Material::ParamTable& params, GLuint& texIndex) const
        {
			// Upload all params
            for (const auto& param : params.bool_params)
            {
                const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
                    glUniform1i(location, param.second ? 1 : 0);
                }
            }

			for (const auto& param : params.float_params)
			{
				const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
				    glUniform1f(location, param.second);
                }
			}

			for (const auto& param : params.vec2_params)
			{
				const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
				    glUniform2fv(location, 1, param.second.vec());
                }
			}

			for (const auto& param : params.vec3_params)
			{
				const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
				    glUniform3fv(location, 1, param.second.vec());
                }
			}

			for (const auto& param : params.vec4_params)
			{
				const auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
				    glUniform4fv(location, 1, param.second.vec());
                }
			}

			for (const auto& param : params.texture_params)
			{
				auto location = get_uniform_location(param.first.c_str());
                if (location != -1)
                {
				    const auto tex = renderState.get_texture_2d_resource(param.first);
				    glActiveTexture(texIndex);
				    glBindTexture(GL_TEXTURE_2D, tex);

                    glUniform1i(location, texIndex);
				    texIndex += 1;
                }
			}
		}

	    GLint GLMaterial::get_uniform_location(const char* name, GLDebugOutputMode out_mode) const
	    {
            const auto location = glGetUniformLocation(_id, name);

            if (location == -1 && out_mode != GLDebugOutputMode::NONE)
            {
                std::cout << "GLRenderSystem: could not locate shader uniform - '" << name << "'" << std::endl;
                return -1;
            }

            if (out_mode == GLDebugOutputMode::ANYTHING)
            {
                std::cout << "GLRenderSystem: uniform '" << name << "'" << std::endl;
            }

            return location;
	    }
	}
}
