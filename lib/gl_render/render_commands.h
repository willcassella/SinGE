#pragma once

#include "lib/gl_render/gl_material.h"
#include "lib/gl_render/gl_render_system.h"

namespace sge
{
    namespace gl_render
    {
        /**
         * \brief Contains per-instance data for meshes that do not override material properties.
         */
        struct RenderCommand_MeshInstance
        {
            Mat4 world_transform;
            Vec2 mat_uv_scale;
            GLuint lightmap_x_basis = 0;
            GLuint lightmap_y_basis = 0;
            GLuint lightmap_z_basis = 0;
            GLuint lightmap_direct_mask = 0;
        };

        struct RenderCommand_Mesh
        {
            GLuint vao = 0;
            GLuint start_element_index = 0;
            GLsizei num_element_indices = 0;
            GLint base_vertex = 0;
        };

        struct RenderCommand_Lines
        {
            GLuint vao = 0;
            GLsizei num_verts = 0;
        };

        /**
         * \brief Sets up and clears the given GBuffer for rendering.
         */
        void render_scene_prepare_gbuffer(
            GLuint gbuffer);

        void RenderCommand_bind_framebuffer(
            const GLuint framebuffer,
            const GLuint width,
            const GLuint height);

        /**
         * \brief Command to bind the given material for rendering.
         * \param program_id The id of the material to bind.
         * \param uniforms Uniform locations for the material.
         * \param params Parameters to set for the material.
         * \param view_matrix The view matrix to set for the material.
         * \param proj_matrix Projection matrix to set for the material.
         */
        void RenderCommand_bind_material(
            const GLuint program_id,
            const gl_material::MaterialStandardUniforms uniforms,
            const gl_material::MaterialParams& params,
            const Mat4& view_matrix,
            const Mat4& proj_matrix);

        /**
         * \brief Renders the given meshes to the currently bound framebuffer with the currently bound material and parameters.
         * \param uniforms Uniform locations for the currently bound material.
         * \param mesh The mesh to render.
         * \param mesh_instances The instances of this mesh to render.
         * \param num_instances The number of mesh instances to render.
         */
        void RenderCommand_render_meshes(
            const gl_material::MaterialStandardUniforms uniforms,
            const RenderCommand_Mesh& mesh,
            const RenderCommand_MeshInstance* mesh_instances,
            std::size_t num_instances);

        /**
         * \brief Command to render sets of lines to the currently bound framebuffer with the currently bound material and parameters.
         * \param lines Sets of lines to draw. NOTE: You can have arbitrarily many lines within the buffer for each 'RenderCommand_Lines' object, which is perferable over multiple commnands.
         * \param num_lines The number of lines to draw.
         */
        void RenderCommand_render_lines(
            const RenderCommand_Lines* lines,
            std::size_t num_lines);

        void render_scene_shade_hdr(
            GLuint framebuffer,
            const GLRenderSystem::State& render_scene,
            Mat4 view);
    }
}
