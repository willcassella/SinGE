// RenderCommands.cpp

#include "../private/RenderCommands.h"
#include "../private/GLRenderSystemState.h"

namespace sge
{
    namespace gl_render
    {
        void render_scene_prepare_gbuffer(GLuint gbuffer)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);

            // Clear the GBuffer
            glEnable(GL_STENCIL_TEST);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glClearDepth(1.f);
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glDepthMask(GL_TRUE);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }

        void RenderCommand_bind_framebuffer(
            const GLuint framebuffer,
            const GLuint width,
            const GLuint height)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glViewport(0, 0, width, height);
        }

        void RenderCommand_bind_material(
            const GLuint program_id,
            const gl_material::MaterialStandardUniforms uniforms,
            const gl_material::MaterialParams& params,
            const Mat4& view_matrix,
            const Mat4& proj_matrix)
        {
            glUseProgram(program_id);

            GLenum next_active_texture = gl_material::FIRST_USER_TEXTURE_SLOT;
            gl_material::set_material_params(program_id, &next_active_texture, params);

            glProgramUniformMatrix4fv(program_id, uniforms.view_matrix_uniform, 1, GL_FALSE, view_matrix.vec());
            glProgramUniformMatrix4fv(program_id, uniforms.proj_matrix_uniform, 1, GL_FALSE, proj_matrix.vec());
        }

        void RenderCommand_render_meshes(
            const gl_material::MaterialStandardUniforms uniforms,
            const RenderCommand_Mesh& mesh,
            const RenderCommand_MeshInstance* instances,
            std::size_t num_instances)
        {
            // Bind the mesh
            glBindVertexArray(mesh.vao);

            for (std::size_t i = 0; i < num_instances; ++i)
            {
                // Set lightmap parameters
                glActiveTexture(gl_material::LIGHTMAP_X_BASIS_TEXTURE_SLOT);
                glBindTexture(GL_TEXTURE_2D, instances[i].lightmap_x_basis);
                glActiveTexture(gl_material::LIGHTMAP_Y_BASIS_TEXTURE_SLOT);
                glBindTexture(GL_TEXTURE_2D, instances[i].lightmap_y_basis);
                glActiveTexture(gl_material::LIGHTMAP_Z_BASIS_TEXTURE_SLOT);
                glBindTexture(GL_TEXTURE_2D, instances[i].lightmap_z_basis);
                glActiveTexture(gl_material::LIGHTMAP_DIRECT_MASK_TEXTURE_SLOT);
                glBindTexture(GL_TEXTURE_2D, instances[i].lightmap_direct_mask);
                glUniform1i(uniforms.use_lightmap_uniform, instances[i].lightmap_x_basis == 0 ? 0 : 1);

                // Set model matrix
                glUniformMatrix4fv(
                    uniforms.model_matrix_uniform,
                    1,
                    GL_FALSE,
                    instances[i].world_transform.vec());

                // Set instance UV Scale
                glUniform2fv(
                    uniforms.inst_mat_uv_scale_uniform,
                    1,
                    instances[i].mat_uv_scale.vec());

                // Draw the mesh
                glDrawRangeElements(
                    GL_TRIANGLES,
                    mesh.start_element_index,
                    mesh.start_element_index + mesh.num_element_indices,
                    mesh.num_element_indices,
                    GL_UNSIGNED_INT,
                    nullptr);
            }
        }

        void RenderCommand_render_lines(
            const RenderCommand_Lines* line_sets,
            std::size_t num_line_sets)
        {
            for (std::size_t i = 0; i < num_line_sets; ++i)
            {
                glBindVertexArray(line_sets[i].vao);
                glDrawArrays(GL_LINES, 0, line_sets[i].num_verts);
            }
        }

        void render_scene_shade_hdr(
            GLuint framebuffer,
            const GLRenderSystem::State& render_state,
            Mat4 view)
        {
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);

            // Bind the screen quad for rasterization (in use for remainder of rendering)
            glBindVertexArray(render_state.sprite_vao);

            // Bind the GBuffer's sub-buffers as textures for reading (in use for remainder of rendering)
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, render_state.gbuffer_layers[GBufferLayer::DEPTH_STENCIL]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, render_state.gbuffer_layers[GBufferLayer::POSITION]);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, render_state.gbuffer_layers[GBufferLayer::NORMAL]);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, render_state.gbuffer_layers[GBufferLayer::ALBEDO]);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, render_state.gbuffer_layers[GBufferLayer::ROUGHNESS_METALLIC]);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, render_state.gbuffer_layers[GBufferLayer::IRRADIANCE]);

            // Bind the given framebuffer for drawing
            const GLenum draw_buffer = GL_COLOR_ATTACHMENT0;
            glBindFramebuffer(GL_FRAMEBUFFER, render_state.post_framebuffer);
            glDrawBuffers(1, &draw_buffer);

            // Bind the scene shading program
            glUseProgram(render_state.scene_shader_program);

            // Upload view matrix
            glUniformMatrix4fv(render_state.scene_program_view_uniform, 1, GL_FALSE, view.vec());

            // Upload light uniforms
            glUniform3fv(render_state.scene_program_light_dir_uniform, 1, render_state.render_scene.light_dir.vec());
            glUniform3fv(render_state.scene_program_light_intensity_uniform, 1, render_state.render_scene.light_intensity.vec());

            // Draw the screen quad
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
}
