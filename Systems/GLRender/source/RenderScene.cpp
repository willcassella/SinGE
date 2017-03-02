// RenderScene.cpp

#include "../private/RenderScene.h"
#include "../private/GLRenderSystemState.h"

namespace sge
{
    namespace gl_render
    {
        void render_scene_prepare_gbuffer(GLuint gbuffer)
        {
            constexpr GLenum GBUFFER_DRAW_BUFFERS[] = {
                GBUFFER_POSITION_ATTACHMENT,
                GBUFFER_NORMAL_ATTACHMENT,
                GBUFFER_ALBEDO_ATTACHMENT,
                GBUFFER_ROUGHNESS_METALLIC_ATTACHMENT,
                GBUFFER_IRRADIANCE_ATTACHMENT };
            constexpr GLsizei NUM_GBUFFER_DRAW_BUFFERS = sizeof(GBUFFER_DRAW_BUFFERS) / sizeof(GLenum);

            // Bind the GBuffer and its sub-buffers for drawing
            glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
            glDrawBuffers(NUM_GBUFFER_DRAW_BUFFERS, GBUFFER_DRAW_BUFFERS);

            // Clear the GBuffer
            glEnable(GL_STENCIL_TEST);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glDepthFunc(GL_LEQUAL);
        }

        void render_scene_fill_bound_gbuffer(
            const RenderScene& scene,
            Mat4 view,
            Mat4 proj)
        {
            // For each material
            for (const auto& mat_instance : scene.standard_material_instances)
            {
                // Bind the material
                glUseProgram(mat_instance.material.program_id);
                glUniform1i(mat_instance.material.uniforms.prec_lightmap_uniform, 0);
                GLenum next_active_texture = GL_TEXTURE1; // Texture 0 is reserved for precomputed irradiance
                gl_material::set_bound_material_params(&next_active_texture, mat_instance.material.params);

                // Upload view and projection matrices
                glUniformMatrix4fv(mat_instance.material.uniforms.view_matrix_uniform, 1, GL_FALSE, view.vec());
                glUniformMatrix4fv(mat_instance.material.uniforms.proj_matrix_uniform, 1, GL_FALSE, proj.vec());

                // Bind the precomputed irradiance texture slot
                glActiveTexture(GL_TEXTURE0);

                // Render each instance of the material that does not override parameters
                for (const auto& mesh_instance : mat_instance.mesh_instances)
                {
                    // Bind the mesh
                    glBindVertexArray(mesh_instance.vao);

                    // For each instance of this mesh
                    for (const auto& instance : mesh_instance.instances)
                    {
                        // Set the precomputed lightmap texture
                        glBindTexture(GL_TEXTURE_2D, instance.lightmap);

                        // Set the model matrix
                        glUniformMatrix4fv(
                            mat_instance.material.uniforms.model_matrix_uniform,
                            1,
                            GL_FALSE,
                            instance.world_transform.vec());

                        // Set the uv scale
                        glUniform2fv(
                            mat_instance.material.uniforms.mat_uv_scale_uniform,
                            1,
                            instance.mat_uv_scale.vec());

                        // Draw the mesh
                        glDrawRangeElements(
                            GL_TRIANGLES,
                            mesh_instance.start_element_index,
                            mesh_instance.start_element_index + mesh_instance.num_element_indices,
                            mesh_instance.num_element_indices,
                            GL_UNSIGNED_INT,
                            nullptr);
                    }
                }

                // Render each instace of the material that overrides parameters
                for (const auto& mesh_instance : mat_instance.param_override_instances)
                {
                    // Bind the mesh
                    glBindVertexArray(mesh_instance.vao);

                    // For each instance of this mesh
                    for (const auto& instance : mesh_instance.instances)
                    {
                        next_active_texture = GL_TEXTURE1;

                        // Rebind the material defaults
                        gl_material::set_bound_material_params(&next_active_texture, mat_instance.material.params);

                        // Bind the instance parameters
                        gl_material::set_bound_material_params(&next_active_texture, instance.override_params);

                        // Set the precomputed irradiance texture
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, instance.lightmap);

                        // Set the uv scale
                        glUniform2fv(mat_instance.material.uniforms.mat_uv_scale_uniform, 1, instance.mat_uv_scale.vec());

                        // Draw the mesh
                        glDrawRangeElements(
                            GL_TRIANGLES,
                            mesh_instance.start_element_index,
                            mesh_instance.start_element_index + mesh_instance.num_element_indices,
                            mesh_instance.num_element_indices,
                            GL_UNSIGNED_INT,
                            nullptr);
                    }
                }
            }
        }

        static void render_lightmask_object(
            const LightmaskObjectInstance& instance,
            Mat4 view,
            Mat4 proj)
        {
            // Bind the material
            glUseProgram(instance.material.program_id);
            GLenum next_active_texture = GL_TEXTURE1;
            gl_material::set_bound_material_params(&next_active_texture, instance.material.params);

            // Bind standard uniforms
            glUniform2fv(instance.material.uniforms.mat_uv_scale_uniform, 1, instance.mat_uv_scale.vec());
            glUniformMatrix4fv(instance.material.uniforms.model_matrix_uniform, 1, GL_FALSE, instance.world_transform.vec());
            glUniformMatrix4fv(instance.material.uniforms.view_matrix_uniform, 1, GL_FALSE, view.vec());
            glUniformMatrix4fv(instance.material.uniforms.proj_matrix_uniform, 1, GL_FALSE, proj.vec());

            // Bind the mesh
            glBindVertexArray(instance.vao);

            // Draw the mesh
            glDrawElements(GL_TRIANGLES, instance.num_element_indices, GL_UNSIGNED_INT, nullptr);
        }

        void render_scene_render_lightmasks(
            const RenderScene& scene,
            Mat4 view,
            Mat4 proj)
        {
            // Enable stencil testing
            glEnable(GL_STENCIL_TEST);

            /*--- DRAW FRONTFACES OF LIGHTMASK RECEIVERS ---*/

            // Disable color and depth output
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            glDepthMask(GL_FALSE);

            // Set stencil
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

            for (const auto& receiver : scene.lightmask_receivers)
            {
                render_lightmask_object(receiver, view, proj);
            }

            /*--- DRAW BACKFACES OF LIGHTMASK RECEIVERS ---*/

            // Draw back faces only
            glCullFace(GL_FRONT);

            // Enable depth output
            glDepthMask(GL_TRUE);

            // Reset iterators
            for (const auto& receiver : scene.lightmask_receivers)
            {
                render_lightmask_object(receiver, view, proj);
            }

            /*--- DRAW BACKFACE OF LIGHTMASK OBSTRUCTORS ---*/

            // Enable color output
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

            // Set to replace stencil buffer with '3' wherever drawn (within backface)
            glStencilFunc(GL_EQUAL, 0x03, 0x01);
            glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

            // Reset iterators
            for (const auto& obstructor : scene.lightmask_obstructors)
            {
                render_lightmask_object(obstructor, view, proj);
            }

            /*--- OVERWRITE DEPTH WITH FRONT FACES OF LIGHTMASK RECEIVERS ---*/

            // Draw front faces only
            glCullFace(GL_BACK);

            // Set stencil to always pass within backface area, allow drawing where depth fails
            glStencilFunc(GL_LEQUAL, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);

            // Disable color output
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

            // Reset iterators
            for (const auto& receiver : scene.lightmask_receivers)
            {
                render_lightmask_object(receiver, view, proj);
            }

            /*--- DRAW FRONT FACES OF LIGHTMASK OBSTRUCTORS ---*/

            // Set to allow drawing where depth test fails
            glStencilFunc(GL_LEQUAL, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);

            // Reset iterators
            for (const auto& obstructor : scene.lightmask_obstructors)
            {
                render_lightmask_object(obstructor, view, proj);
            }

            /*--- DRAW FRONT FACES OF LIGTHMASK RECEIVERS ---*/

            // Set depth function to always pass
            glDepthFunc(GL_ALWAYS);

            // Set stencil to always pass within backface area
            glStencilFunc(GL_EQUAL, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

            // Enable color output
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

            // Reset iterators
            for (const auto& receiver : scene.lightmask_receivers)
            {
                render_lightmask_object(receiver, view, proj);
            }
        }

        void render_scene_shade_hdr(
            GLuint framebuffer,
            const GLRenderSystem::State& render_state,
            Mat4 view)
        {
            /*-----------------------*/
            /*---   PBR SHADING   ---*/

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

            // Draw the screen quad
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
}
