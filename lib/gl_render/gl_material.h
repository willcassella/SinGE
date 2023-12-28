#pragma once

#include "lib/base/math/mat4.h"
#include "lib/base/math/vec4.h"
#include "lib/gl_render/util.h"

namespace sge {
namespace gl_render {
namespace gl_material {
constexpr GLint POSITION_ATTRIB_LOCATION = 0;
constexpr GLint NORMAL_ATTRIB_LOCATION = 1;
constexpr GLint TANGENT_ATTRIB_LOCATION = 2;
constexpr GLint BITANGENT_SIGN_ATTRIB_LOCATION = 3;
constexpr GLint MATERIAL_TEXCOORD_ATTRIB_LOCATION = 4;
constexpr GLint LIGHTMAP_TEXCOORD_ATTRIB_LOCATION = 5;
constexpr const char* POSITION_ATTRIB_NAME = "v_position";
constexpr const char* NORMAL_ATTRIB_NAME = "v_normal";
constexpr const char* TANGENT_ATTRIB_NAME = "v_tangent";
constexpr const char* BITANGENT_SIGN_ATTRIB_NAME = "v_bitangent_sign";
constexpr const char* MATERIAL_TEXCOORD_ATTRIB_NAME = "v_mat_texcoord";
constexpr const char* LIGHTMAP_TEXCOORD_ATTRIB_NAME = "v_lm_texcoord";
constexpr const char* MODEL_MATRIX_UNIFORM_NAME = "model";
constexpr const char* VIEW_MATRIX_UNIFORM_NAME = "view";
constexpr const char* PROJ_MATRIX_UNIFORM_NAME = "projection";
constexpr const char* BASE_MAT_UV_SCALE_UNIFORM_NAME = "base_mat_uv_scale";
constexpr const char* INST_MAT_UV_SCALE_UNIFORM_NAME = "inst_mat_uv_scale";
constexpr const char* LIGHTMAP_X_BASIS_UNIFORM_NAME = "lightmap_x_basis";
constexpr const char* LIGHTMAP_Y_BASIS_UNIFORM_NAME = "lightmap_y_basis";
constexpr const char* LIGHTMAP_Z_BASIS_UNIFORM_NAME = "lightmap_z_basis";
constexpr const char* LIGHTMAP_DIRECT_MASK_UNIFORM_NAME = "lightmap_direct_mask";
constexpr const char* USE_LIGHTMAP_UNIFORM_NAME = "use_lightmap";
constexpr GLenum LIGHTMAP_X_BASIS_TEXTURE_SLOT = GL_TEXTURE0;
constexpr GLenum LIGHTMAP_Y_BASIS_TEXTURE_SLOT = GL_TEXTURE1;
constexpr GLenum LIGHTMAP_Z_BASIS_TEXTURE_SLOT = GL_TEXTURE2;
constexpr GLenum LIGHTMAP_DIRECT_MASK_TEXTURE_SLOT = GL_TEXTURE3;
constexpr GLenum FIRST_USER_TEXTURE_SLOT = GL_TEXTURE4;

/**
 * \brief Material parameters.
 */
struct MaterialParams {
  std::map<GLint, GLint> int_params;
  std::map<GLint, float> float_params;
  std::map<GLint, Vec2> vec2_params;
  std::map<GLint, Vec3> vec3_params;
  std::map<GLint, Vec4> vec4_params;
  std::map<GLint, GLuint> tex_params;
};

/**
 * \brief Location of standard material uniforms.
 */
struct MaterialStandardUniforms {
  GLint model_matrix_uniform = -1;
  GLint view_matrix_uniform = -1;
  GLint proj_matrix_uniform = -1;
  GLint base_mat_uv_scale_uniform = -1;
  GLint inst_mat_uv_scale_uniform = -1;
  GLint lightmap_x_basis_uniform = -1;
  GLint lightmap_y_basis_uniform = -1;
  GLint lightmap_z_basis_uniform = -1;
  GLint lightmap_direct_mask_uniform = -1;
  GLint use_lightmap_uniform = -1;
};

/**
 * \brief A complete material object.
 */
struct Material {
  GLuint program_id = 0;
  MaterialStandardUniforms uniforms;
  MaterialParams params;
};

/**
 * \brief Creates a standard material shader program with the given vertex shader and fragment shader.
 * Binds standard vertex attribute locations as well.
 * \param v_shader The vertex shader for this material.
 * \param f_shader The fragment shader for this material
 * \return The id of the newly created shader program. The user is responsible for checking program link
 * status.
 */
GLuint new_standard_material_program(GLuint v_shader, GLuint f_shader);

/**
 * \brief Frees resources associated with the given standard material.
 * \param mat_id The material to free.
 */
void free_standard_material_program(GLuint mat_id);

/**
 * \brief Gets the uniform locations for the given material.
 * \param mat_id The id of the material to get the uniform locations for.
 * \param out_uniforms Structure to assign the uniform locations to.
 */
void get_material_standard_uniforms(GLuint mat_id, MaterialStandardUniforms* out_uniforms);

/**
 * \brief Gets the uniform location for the given material, optionally outputting an error.
 * \param mat_id The material to get the uniform location for.
 * \param name The name of the uniform.
 * \param out_mode Error output options.
 * \return The uniform location.
 */
GLint get_uniform_location(
    GLuint mat_id,
    const char* name,
    GLDebugOutputMode out_mode = GLDebugOutputMode::ONLY_ERROR
);

/**
 * \brief Initializes the lightmap parameters of the given material to the standard values (specified above).
 * You only need to call this once per material. \param mat_id The id of the material. \param uniforms Uniform
 * locations for the currently bound material.
 */
void init_material_lightmap_params(const GLuint mat_id, const MaterialStandardUniforms uniforms);

/**
 * \brief Sets the given material parameters for the given material.
 * \param mat_id The id of the material.
 * \param next_active_texture The next texture slot to use for texture parameters.
 * \param params The parameters for the material.
 */
void set_material_params(
    const GLuint mat_id,
    GLenum* const next_active_texture,
    const MaterialParams& params
);
}  // namespace gl_material
}  // namespace gl_render
}  // namespace sge
