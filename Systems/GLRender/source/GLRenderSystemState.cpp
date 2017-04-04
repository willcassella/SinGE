// GLRenderSystemState.cpp

#include <fstream>
#include <Resource/Resources/Material.h>
#include <Resource/Resources/StaticMesh.h>
#include <Resource/Resources/Texture.h>
#include <Resource/Resources/HDRImage.h>
#include <Core/Util/StringUtils.h>
#include <Engine/Util/DebugDraw.h>
#include "../private/GLRenderSystemState.h"

namespace sge
{
	namespace gl_render
	{
		const gl_material::Material& GLRenderSystem::State::get_material_resource(const std::string& path)
		{
			auto iter = material_resources.find(path);
            if (iter == material_resources.end())
            {
                // Create the material from the archive
                Material material;
                const auto loaded = material.from_file(path.c_str());

                // If the material could not be loaded, return missing material
                if (!loaded)
                {
                    return missing_material;
                }

                // Load shaders required by this material
                const auto v_shader = get_shader_resource(material.vertex_shader());
                const auto f_shader = get_shader_resource(material.pixel_shader());

                // Create the material
                gl_material::Material gl_mat;
                gl_mat.program_id = gl_material::new_standard_material_program(v_shader, f_shader);
                debug_program_status(gl_mat.program_id, GLDebugOutputMode::ONLY_ERROR);

                // Get standard uniforms for the material program
                gl_material::get_material_standard_uniforms(gl_mat.program_id, &gl_mat.uniforms);

				// Set base uv scale
				glUseProgram(gl_mat.program_id);
				glUniform2fv(gl_mat.uniforms.base_mat_uv_scale_uniform, 1, material.base_uv_scale().vec());
				glUseProgram(0);

                // Get all of the default int parameters for this material
                for (const auto& int_param : material.param_table().bool_params)
                {
                    const auto uniform_loc = gl_material::get_uniform_location(gl_mat.program_id, int_param.first.c_str());
                    if (uniform_loc != -1)
                    {
                        gl_mat.params.int_params[uniform_loc] = int_param.second ? 1 : 0;
                    }
                }

                // Get all default float parameters
                for (const auto& float_param : material.param_table().float_params)
                {
                    const auto uniform_loc = gl_material::get_uniform_location(gl_mat.program_id, float_param.first.c_str());
                    if (uniform_loc != -1)
                    {
                        gl_mat.params.float_params[uniform_loc] = float_param.second;
                    }
                }

                // Get all default Vec2 parameters
                for (const auto& vec2_param : material.param_table().vec2_params)
                {
                    const auto uniform_loc = gl_material::get_uniform_location(gl_mat.program_id, vec2_param.first.c_str());
                    if (uniform_loc != -1)
                    {
                        gl_mat.params.vec2_params[uniform_loc] = vec2_param.second;
                    }
                }

                // Get all default Vec3 parameters
                for (const auto& vec3_param : material.param_table().vec3_params)
                {
                    const auto uniform_loc = gl_material::get_uniform_location(gl_mat.program_id, vec3_param.first.c_str());
                    if (uniform_loc != -1)
                    {
                        gl_mat.params.vec3_params[uniform_loc] = vec3_param.second;
                    }
                }

                // Get all default Vec4 parameters
                for (const auto& vec4_param : material.param_table().vec4_params)
                {
                    const auto uniform_loc = gl_material::get_uniform_location(gl_mat.program_id, vec4_param.first.c_str());
                    if (uniform_loc != -1)
                    {
                        gl_mat.params.vec4_params[uniform_loc] = vec4_param.second;
                    }
                }

                // Get all texture parameters
                for (const auto& tex_param : material.param_table().texture_params)
                {
                    const auto uniform_loc = gl_material::get_uniform_location(gl_mat.program_id, tex_param.first.c_str());
                    if (uniform_loc != -1)
                    {
                        // Get the texture resource
                        const auto tex_id = get_texture_2d_resource(tex_param.second, false);
                        gl_mat.params.tex_params[uniform_loc] = tex_id;
                    }
                }

                // Put it into the resource table
                iter = material_resources.insert(std::make_pair(path, std::move(gl_mat))).first;
            }

		    return iter->second;
		}

        const gl_static_mesh::StaticMesh& GLRenderSystem::State::get_static_mesh_resource(const std::string& path)
        {
            auto iter = static_mesh_resources.find(path);
            if (iter == static_mesh_resources.end())
            {
                // Load the mesh from the file
                StaticMesh static_mesh;
                const bool loaded = static_mesh.from_file(path.c_str());

                // If the mesh could not be loaded, return the missing mesh object
                if (!loaded)
                {
                    return missing_mesh;
                }

                // Create a GLStaticMesh from the loaded mesh object
                gl_static_mesh::StaticMesh gl_mesh;
                glGenVertexArrays(1, &gl_mesh.vao);
                glGenBuffers(1, &gl_mesh.ebo);
                glGenBuffers(gl_static_mesh::NUM_VERTEX_BUFFERS, gl_mesh.vertex_buffers.data());

                // Upload data
                gl_static_mesh::upload_static_mesh_vertex_data(
                    gl_mesh.vao,
                    gl_mesh.vertex_buffers.data(),
                    static_mesh.num_verts(),
                    static_mesh.vertex_positions(),
                    static_mesh.vertex_normals(),
                    static_mesh.vertex_tangents(),
                    static_mesh.bitangent_signs(),
                    static_mesh.material_uv(),
                    static_mesh.lightmap_uv());
                gl_static_mesh::upload_static_mesh_elements(
                    gl_mesh.vao,
                    gl_mesh.ebo,
                    static_mesh.num_triangle_elements(),
                    static_mesh.triangle_elements());
                gl_mesh.num_total_elements = static_cast<GLint>(static_mesh.num_triangle_elements());

                // Create each material slice for the mesh
                for (std::size_t i = 0; i < static_mesh.num_materials(); ++i)
                {
                    // Get the material
                    const auto& mat = static_mesh.materials()[i];
                    const auto& gl_mat = get_material_resource(mat.path());

                    // Create the slice
                    gl_static_mesh::MeshSlice slice;
                    slice.material = gl_mat.program_id;
                    slice.start_element_index = mat.start_elem_index();
                    slice.num_element_indices = mat.num_elem_indices();
                    gl_mesh.material_slices.push_back(slice);
                }

                // Insert it into the resource table
                iter = static_mesh_resources.insert(std::make_pair(path, std::move(gl_mesh))).first;
            }

            return iter->second;
        }

        GLShader::Id GLRenderSystem::State::get_shader_resource(const std::string& path)
        {
            auto iter = shader_resources.find(path);
            if (iter == shader_resources.end())
            {
                // Figure out the type of the shader
                GLenum type;
                if (string_ends_with(path, ".vert"))
                {
                    type = GL_VERTEX_SHADER;
                }
                else if (string_ends_with(path, ".frag"))
                {
                    type = GL_FRAGMENT_SHADER;
                }
                else
                {
                    assert(false);
                    return 0;
                }

                // Create the shader
                GLShader shader{ type, path };
                const auto id = shader.id();

                // Put it into the resource table
                shader_resources.insert(std::make_pair(path, std::move(shader)));
                return id;
            }
            else
            {
                return iter->second.id();
            }
        }

		GLuint GLRenderSystem::State::get_texture_2d_resource(const std::string& path, bool hdr)
		{
			auto iter = texture_2d_resources.find(path);
			if (iter == texture_2d_resources.end())
			{
                if (!hdr)
                {
                    // Load the texture from the file
                    Texture texture;
                    const auto loaded = texture.from_file(path.c_str());

                    // Figure out the color format of the image
                    GLenum format;
                    switch (texture.color_space())
                    {
                    case Texture::ColorSpace::RGB:
                        format = GL_RGBA8;
                        break;

                    case Texture::ColorSpace::S_RGB:
                        format = GL_SRGB8_ALPHA8;
                        break;

                    default:
                        assert(false);
                        return 0;
                    }

                    // Create an opengl texture from the texture object
                    const auto gl_tex = create_texture(
                        texture.image.get_width(),
                        texture.image.get_height(),
                        texture.image.get_bitmap(),
                        format,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE);

                    // Add it to the resource table
                    texture_2d_resources.insert(std::make_pair(path, gl_tex));
                    return gl_tex;
                }
                else
                {
                    HDRImage image;
                    const auto loaded = image.from_file(path.c_str());

                    // Figure out which internal and upload format to use
                    GLenum internal_format;
                    GLenum upload_format;
                    switch (image.get_num_channels())
                    {
                    case 3:
                        internal_format = GL_RGB32F;
                        upload_format = GL_RGB;
                        break;

                    case 4:
                        internal_format = GL_RGBA32F;
                        upload_format = GL_RGBA;
                        break;

                    default:
                        assert(false);
                        return 0;
                    }

                    // Create an opengl texture for this image
                    const auto gl_tex = create_texture(
                        image.get_width(),
                        image.get_height(),
                        image.get_bits(),
                        internal_format,
                        upload_format,
                        GL_FLOAT);

                    // Add it to the resource table
                    texture_2d_resources.insert(std::make_pair(path, gl_tex));
                    return gl_tex;
                }
			}
			else
			{
				return iter->second;
			}
		}

		void GLRenderSystem::State::gather_debug_lines(EventChannel& debug_line_channel, EventChannel::SubscriberId subscriber_id)
		{
			// Clear current lines
			frame_debug_lines.clear();

			DebugLine lines[64];
			int32 num_lines;
			DebugLineVert verts[128];
			while (debug_line_channel.consume(subscriber_id, lines, &num_lines))
			{
				// Fill in lines
				for (int32 i = 0; i < num_lines; ++i)
				{
					verts[i * 2].color_rgb = lines[i].color;
					verts[i * 2].world_position = lines[i].world_start;
					verts[i * 2 + 1].color_rgb = lines[i].color;
					verts[i * 2 + 1].world_position = lines[i].world_end;
				}

				// Add them to the buffer
				frame_debug_lines.insert(frame_debug_lines.end(), &verts[0], &verts[0] + num_lines * 2);
			}
		}
	}
}
