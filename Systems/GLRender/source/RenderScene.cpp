// RenderScene.cpp

#include <Engine/SystemFrame.h>
#include <Engine/Util/VectorUtils.h>
#include <Engine/Components/CTransform3D.h>
#include <Engine/Components/Display/CStaticMesh.h>
#include <Engine/Components/Display/CLightMaskObstructor.h>
#include <Engine/Components/Display/CLightMaskReceiver.h>
#include "../private/RenderScene.h"
#include "../private/GLRenderSystemState.h"

namespace sge
{
    namespace gl_render
    {
        static void render_prepare(
            const GLStaticMesh& mesh,
            const GLMaterial& mat,
            const Mat4& model,
            const Mat4& view,
            const Mat4& proj)
        {
            // Bind the mesh and material
            mesh.bind();
            GLuint texIndex = GL_TEXTURE0;
            mat.bind(texIndex);

            // Upload transformation matrices
            mat.set_model_matrix(model);
            mat.set_view_matrix(view);
            mat.set_projection_matrix(proj);
        }

        void render_scene_init(GLRenderSystem::State& state, SystemFrame& frame)
        {
            auto& render_scene = state.render_scene;
            render_scene.ord_render_entities.clear();
            render_scene.ord_render_entities_matrices.clear();

            // Get all entities with a transform
            frame.process_entities(
                [&render_scene] (
                    ProcessingFrame& pframe,
                    const CTransform3D& transform)
            {
                render_scene.ord_render_entities.push_back(pframe.entity());
                render_scene.ord_render_entities_matrices.push_back(transform.get_world_matrix());
            });

            render_scene.ord_mesh_entities.clear();
            render_scene.ord_mesh_entity_meshes.clear();
            render_scene.ord_mesh_entity_materials.clear();

            // Get all entities with a static mesh component
            frame.process_entities(
                [&render_scene, &state] (
                    ProcessingFrame& pframe,
                    const CStaticMesh& static_mesh)
            {
                render_scene.ord_mesh_entities.push_back(pframe.entity());
                render_scene.ord_mesh_entity_meshes.push_back(state.get_static_mesh_resource(static_mesh.mesh()));
                render_scene.ord_mesh_entity_materials.push_back(state.get_material_resource(static_mesh.material()));
            });

            render_scene.ord_lightmask_obstructors.clear();

            // Get all entities the 'CLightMaskObstructor' component
            frame.process_entities(
                [&render_scene, &state] (
                    ProcessingFrame& pframe,
                    const CLightMaskObstructor& /*obstructor*/)
            {
                render_scene.ord_lightmask_obstructors.push_back(pframe.entity());
            });

            render_scene.ord_lightmask_receivers.clear();

            // Get all entities with the 'CLightMaskReceiver' component
            frame.process_entities(
                [&render_scene, &state] (
                    ProcessingFrame& pframe,
                    const CLightMaskReceiver& /*receiver*/)
            {
                render_scene.ord_lightmask_receivers.push_back(pframe.entity());
            });
        }

        void render_scene_update_matrices(GLRenderSystem::State& state, SystemFrame& frame)
        {
            frame.process_entities(zip(ord_ents_range(state.render_scene.ord_render_entities)),
                [&mats = state.render_scene.ord_render_entities_matrices] (
                    ProcessingFrame& pframe,
                    const CTransform3D& transform)
            {
                mats[pframe.user_iterator(0)] = transform.get_world_matrix();
            });
        }

        void render_scene_render_normal(GLRenderSystem::State& state, Mat4 view, Mat4 proj)
        {
            const auto& render_scene = state.render_scene;

            std::size_t iters[] = { 0, 0, 0, 0 };
            auto& obj_iter = iters[0], &mesh_iter = iters[1], &recv_iter = iters[2], &obst_iter = iters[3];

            const EntityId* const ord_arrays[] = {
                render_scene.ord_render_entities.data(),
                render_scene.ord_mesh_entities.data(),
                render_scene.ord_lightmask_receivers.data(),
                render_scene.ord_lightmask_obstructors.data(),
            };

            const std::size_t ord_array_lens[] = {
                render_scene.ord_render_entities.size(),
                render_scene.ord_mesh_entities.size(),
                render_scene.ord_lightmask_receivers.size(),
                render_scene.ord_lightmask_obstructors.size(),
            };

            // Disable stencil testing
            glDisable(GL_STENCIL_TEST);

            // Cull back faces
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            // User normal depth testing
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            // Draw all normal objects
            while (const auto entity = ord_entities_match(ord_arrays, ord_array_lens, iters, 2, 2, 4))
            {
                // Get the object properties
                const auto& model = render_scene.ord_render_entities_matrices[obj_iter];
                const auto& mesh = state.static_meshes.find(render_scene.ord_mesh_entity_meshes[mesh_iter])->second;
                const auto& material = state.materials.find(render_scene.ord_mesh_entity_materials[mesh_iter])->second;

                // Prepare for rendering
                render_prepare(mesh, material, model, view, proj);

                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());

                // Increment iter
                ++obj_iter;
            }
        }

        void render_scene_render_lightmasks(GLRenderSystem::State& state, Mat4 view, Mat4 proj)
        {
            auto& render_scene = state.render_scene;

            std::size_t iters[] = { 0, 0, 0 };
            auto& obj_iter = iters[0], &mesh_iter = iters[1];

            const EntityId* const recv_pass_ord_arrays[] = {
                render_scene.ord_render_entities.data(),
                render_scene.ord_mesh_entities.data(),
                render_scene.ord_lightmask_receivers.data() };

            const EntityId* const obst_pass_ord_arrays[] = {
                render_scene.ord_render_entities.data(),
                render_scene.ord_mesh_entities.data(),
                render_scene.ord_lightmask_obstructors.data()
            };

            const std::size_t recv_pass_ord_array_lens[] = {
                render_scene.ord_render_entities.size(),
                render_scene.ord_mesh_entities.size(),
                render_scene.ord_lightmask_receivers.size()
            };

            const std::size_t obst_pass_ord_array_lens[] = {
                render_scene.ord_render_entities.size(),
                render_scene.ord_mesh_entities.size(),
                render_scene.ord_lightmask_obstructors.size()
            };

            // Enable stencil testing
            glEnable(GL_STENCIL_TEST);

            /*--- DRAW FRONTFACES OF LIGHTMASK RECEIVERS ---*/

            // Disable color and depth output
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            glDepthMask(GL_FALSE);

            // Set stencil
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

            while (const auto entity = ord_entities_match(recv_pass_ord_arrays, recv_pass_ord_array_lens, iters, 3, 0, 3))
            {
                const auto& model = render_scene.ord_render_entities_matrices[obj_iter];
                const auto& mesh = state.static_meshes.find(render_scene.ord_mesh_entity_meshes[mesh_iter])->second;
                const auto& material = state.materials.find(render_scene.ord_mesh_entity_materials[mesh_iter])->second;

                // Prepare for rendering
                render_prepare(mesh, material, model, view, proj);

                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());

                // Increment iter
                ++obj_iter;
            }

            /*--- DRAW BACKFACES OF LIGHTMASK RECEIVERS ---*/

            // Draw back faces only
            glCullFace(GL_FRONT);

            // Enable depth output
            glDepthMask(GL_TRUE);

            // Reset iterators
            std::memset(iters, 0, sizeof(iters));
            while (const auto entity = ord_entities_match(recv_pass_ord_arrays, recv_pass_ord_array_lens, iters, 3, 0, 3))
            {
                const auto& model = render_scene.ord_render_entities_matrices[obj_iter];
                const auto& mesh = state.static_meshes.find(render_scene.ord_mesh_entity_meshes[mesh_iter])->second;
                const auto& material = state.materials.find(render_scene.ord_mesh_entity_materials[mesh_iter])->second;

                // Prepare for rendering
                render_prepare(mesh, material, model, view, proj);

                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());

                // Increment iter
                ++obj_iter;
            }

            /*--- DRAW BACKFACE OF LIGHTMASK OBSTRUCTORS ---*/

            // Enable color output
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

            // Set to replace stencil buffer with '3' wherever drawn (within backface)
            glStencilFunc(GL_EQUAL, 0x03, 0x01);
            glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

            // Reset iterators
            std::memset(iters, 0, sizeof(iters));
            while (const auto entity = ord_entities_match(obst_pass_ord_arrays, obst_pass_ord_array_lens, iters, 3, 0, 3))
            {
                const auto& model = render_scene.ord_render_entities_matrices[obj_iter];
                const auto& mesh = state.static_meshes.find(render_scene.ord_mesh_entity_meshes[mesh_iter])->second;
                const auto& material = state.materials.find(render_scene.ord_mesh_entity_materials[mesh_iter])->second;

                // Prepare for rendering
                render_prepare(mesh, material, model, view, proj);

                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());

                // Increment iter
                ++obj_iter;
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
            std::memset(iters, 0, sizeof(iters));
            while (const auto entity = ord_entities_match(recv_pass_ord_arrays, recv_pass_ord_array_lens, iters, 3, 0, 3))
            {
                const auto& model = render_scene.ord_render_entities_matrices[obj_iter];
                const auto& mesh = state.static_meshes.find(render_scene.ord_mesh_entity_meshes[mesh_iter])->second;
                const auto& material = state.materials.find(render_scene.ord_mesh_entity_materials[mesh_iter])->second;

                // Prepare for rendering
                render_prepare(mesh, material, model, view, proj);


                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());

                // Increment iter
                ++obj_iter;
            }

            /*--- DRAW FRONT FACES OF LIGHTMASK OBSTRUCTORS ---*/

            // Set to allow drawing where depth test fails
            glStencilFunc(GL_LEQUAL, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);

            // Reset iterators
            std::memset(iters, 0, sizeof(iters));
            while (const auto entity = ord_entities_match(obst_pass_ord_arrays, obst_pass_ord_array_lens, iters, 3, 0, 3))
            {
                const auto& model = render_scene.ord_render_entities_matrices[obj_iter];
                const auto& mesh = state.static_meshes.find(render_scene.ord_mesh_entity_meshes[mesh_iter])->second;
                const auto& material = state.materials.find(render_scene.ord_mesh_entity_materials[mesh_iter])->second;

                // Prepare for rendering
                render_prepare(mesh, material, model, view, proj);

                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());

                // Increment iter
                ++obj_iter;
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
            std::memset(iters, 0, sizeof(iters));
            while (const auto entity = ord_entities_match(recv_pass_ord_arrays, recv_pass_ord_array_lens, iters, 3, 0, 3))
            {
                const auto& model = render_scene.ord_render_entities_matrices[obj_iter];
                const auto& mesh = state.static_meshes.find(render_scene.ord_mesh_entity_meshes[mesh_iter])->second;
                const auto& material = state.materials.find(render_scene.ord_mesh_entity_materials[mesh_iter])->second;

                // Prepare for rendering
                render_prepare(mesh, material, model, view, proj);

                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());

                // Increment iter
                ++obj_iter;
            }
        }
    }
}
