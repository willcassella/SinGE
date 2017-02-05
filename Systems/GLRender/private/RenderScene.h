// RenderScene.h
#pragma once

#include "GLMaterial.h"
#include "GLStaticMesh.h"

namespace sge
{
    namespace gl_render
    {
        struct RenderScene
        {
            //////////////////
            ///   Fields   ///
        public:

            /* All entities that may be considered by the renderer. */
            std::vector<EntityId> ord_render_entities;
            std::vector<Mat4> ord_render_entities_matrices;

            /* All objects that have a static mesh component. */
            std::vector<EntityId> ord_mesh_entities;
            std::vector<GLStaticMesh::VAO> ord_mesh_entity_meshes;
            std::vector<GLMaterial::Id> ord_mesh_entity_materials;

            /* All lightmask obstructor objects. */
            std::vector<EntityId> ord_lightmask_obstructors;

            /* All lightmask receiver objects. */
            std::vector<EntityId> ord_lightmask_receivers;
        };

        void init_render_scene(GLRenderSystem::State& state, SystemFrame& frame);

        void render_scene_render_normal(GLRenderSystem::State& state, Mat4 view, Mat4 proj);

        void render_scene_render_lightmasks(GLRenderSystem::State& state, Mat4 view, Mat4 proj);
    }
}
