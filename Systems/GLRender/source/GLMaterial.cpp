// GLMaterial.cpp

#include <iostream>
#include "../private/GLMaterial.h"

namespace sge
{
	namespace gl_render
	{
        namespace gl_material
        {
            GLuint new_standard_material_program(GLuint v_shader, GLuint f_shader)
            {
                const auto mat_id = glCreateProgram();

                // Attach shaders
                glAttachShader(mat_id, v_shader);
                glAttachShader(mat_id, f_shader);

                // Bind shader attributes
                glBindAttribLocation(mat_id, POSITION_ATTRIB_LOCATION, POSITION_ATTRIB_NAME);
                glBindAttribLocation(mat_id, NORMAL_ATTRIB_LOCATION, NORMAL_ATTRIB_NAME);
                glBindAttribLocation(mat_id, TANGENT_ATTRIB_LOCATION, TANGENT_ATTRIB_NAME);
                glBindAttribLocation(mat_id, BITANGENT_SIGN_ATTRIB_LOCATION, BITANGENT_SIGN_ATTRIB_NAME);
                glBindAttribLocation(mat_id, MATERIAL_TEXCOORD_ATTRIB_LOCATION, MATERIAL_TEXCOORD_ATTRIB_NAME);
                glBindAttribLocation(mat_id, LIGHTMAP_TEXCOORD_ATTRIB_LOCATION, LIGHTMAP_TEXCOORD_ATTRIB_NAME);

                // Link the program
                glLinkProgram(mat_id);

                // Detach the shaders, so they may be deleted independently
                glDetachShader(mat_id, v_shader);
                glDetachShader(mat_id, f_shader);

                return mat_id;
            }

            void free_standard_material_program(GLuint mat_id)
            {
                glDeleteProgram(mat_id);
            }

            void get_material_standard_uniforms(GLuint mat_id, MaterialStandardUniforms* out_uniforms)
            {
                out_uniforms->model_matrix_uniform = glGetUniformLocation(mat_id, MODEL_MATRIX_UNIFORM_NAME);
                out_uniforms->view_matrix_uniform = glGetUniformLocation(mat_id, VIEW_MATRIX_UNIFORM_NAME);
                out_uniforms->proj_matrix_uniform = glGetUniformLocation(mat_id, PROJ_MATRIX_UNIFORM_NAME);
                out_uniforms->mat_uv_scale_uniform = glGetUniformLocation(mat_id, MAT_UV_SCALE_UNIFORM_NAME);
                out_uniforms->prec_lightmap_uniform = glGetUniformLocation(mat_id, PREC_LIGHTMAP_UNIFORM_NAME);
            }

            GLint get_uniform_location(GLuint mat_id, const char* name, GLDebugOutputMode out_mode)
            {
                const auto location = glGetUniformLocation(mat_id, name);

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

            void set_bound_material_params(GLenum* next_active_texture, const MaterialParams& params)
            {
                // Bind int params
                for (auto int_param : params.int_params)
                {
                    glUniform1i(int_param.first, int_param.second);
                }

                // Bind float params
                for (auto float_param : params.float_params)
                {
                    glUniform1f(float_param.first, float_param.second);
                }

                // Bind vec2 params
                for (auto vec2_param : params.vec2_params)
                {
                    glUniform2fv(vec2_param.first, 1, vec2_param.second.vec());
                }

                // Bind vec3 params
                for (auto vec3_param : params.vec3_params)
                {
                    glUniform3fv(vec3_param.first, 1, vec3_param.second.vec());
                }

                // Bind vec4 params
                for (auto vec4_param : params.vec4_params)
                {
                    glUniform4fv(vec4_param.first, 1, vec4_param.second.vec());
                }

                // Bind texture params
                for (auto tex_param : params.tex_params)
                {
                    glActiveTexture(*next_active_texture);
                    glBindTexture(GL_TEXTURE_2D, tex_param.second);
                    glUniform1i(tex_param.first, *next_active_texture - GL_TEXTURE0);
                    *next_active_texture += 1;
                }
            }
        }
	}
}
