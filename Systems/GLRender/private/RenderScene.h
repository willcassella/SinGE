// RenderScene.h
#pragma once

#include "GLMaterial.h"
#include "../include/GLRender/GLRenderSystem.h"

namespace sge
{
    namespace gl_render
    {
        /**
         * \brief Contains per-instance data for meshes that do not override material properties.
         */
        struct MeshInstance
        {
            Mat4 world_transform;
            Vec2 mat_uv_scale;
			NodeId node_id;
            GLuint lightmap_x_basis = 0;
			GLuint lightmap_y_basis = 0;
			GLuint lightmap_z_basis = 0;
			GLuint lightmap_direct_mask = 0;
        };

        /**
         * \brief Contains per-mesh data for meshes that do not override material properties.
         */
        struct MeshInstanceSet
        {
            GLuint vao = 0;
            GLuint start_element_index = 0;
            GLuint num_element_indices = 0;
            std::vector<MeshInstance> instances;
        };

        /**
         * \brief Per-instance data for meshes that override their material parameters.
         */
        struct MaterialOverrideMeshInstance
        {
            Mat4 world_transform;
            Vec2 mat_uv_scale;
			NodeId node_id;
			GLuint lightmap_x_basis = 0;
			GLuint lightmap_y_basis = 0;
			GLuint lightmap_z_basis = 0;
			GLuint lightmap_direct_mask = 0;
            gl_material::MaterialParams override_params;
        };

        /**
         * \brief Contains per-mesh data for meshes that override their material parameters.
         */
        struct MeshInstanceMaterialOverrideSet
        {
            GLuint vao = 0;
            GLuint start_element_index = 0;
            GLuint num_element_indices = 0;
            std::vector<MaterialOverrideMeshInstance> instances;
        };

        struct MaterialInstance
        {
            gl_material::Material material;
            std::vector<MeshInstanceSet> mesh_instances;
            std::vector<MeshInstanceMaterialOverrideSet> param_override_instances;
        };

        /**
         * \brief Per-instance data for lightmask objects (receivers and obstructors).
         */
        struct LightmaskObjectInstance
        {
            gl_material::Material material;
            GLuint vao;
            GLuint num_element_indices;
            Mat4 world_transform;
            Vec2 mat_uv_scale;
        };

        struct RenderScene
        {
            /**
             * \brief Instances of standard materials active in this scene.
             */
            std::vector<MaterialInstance> standard_material_instances;

            /**
             * \brief All lightmask receiver objects in the scene.
             */
            std::vector<LightmaskObjectInstance> lightmask_receivers;

            /**
             * \brief All lightmask obstructor objects in the scene.
             */
            std::vector<LightmaskObjectInstance> lightmask_obstructors;
        };

        /**
         * \brief Sets up and clears the given GBuffer for rendering.
         */
        void render_scene_prepare_gbuffer(
            GLuint gbuffer);

        void render_scene_fill_bound_gbuffer(
            const RenderScene& scene,
            Mat4 view,
            Mat4 proj);

        void render_scene_render_lightmasks(
            const RenderScene& scene,
            Mat4 view,
            Mat4 proj);

        void render_scene_shade_hdr(
            GLuint framebuffer,
            const GLRenderSystem::State& render_scene,
            Mat4 view);
    }
}
