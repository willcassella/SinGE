// RenderScene.h
#pragma once

#include "RenderCommands.h"

namespace sge
{
	struct CStaticMesh;

	namespace gl_render
	{
		struct RenderResource;

		struct RenderScene_Lightmap
		{
			GLuint x_basis_tex = 0;
			GLuint y_basis_tex = 0;
			GLuint z_basis_tex = 0;
			GLuint direct_mask_tex = 0;
		};

		struct RenderScene_Mesh
		{
			/**
			 * \brief Command object for the mesh to render.
			 */
			RenderCommand_Mesh mesh_command;

			/**
			 * \brief Command objects for each instance to be rendered.
			 */
			std::vector<RenderCommand_MeshInstance> instance_commands;

			/**
			 * \brief Array symmetrical with 'instance_commands', stores the ids for each node.
			 */
			std::vector<NodeId> node_ids;
		};

		struct RenderScene_Material
		{
			gl_material::Material material;
			std::vector<RenderScene_Mesh> mesh_instances;
			std::map<GLuint, std::size_t> mesh_indices;
		};

		struct RenderScene_Commands
		{
			/**
			 * \brief Rendering commands for all standard path objects.
			 */
			std::vector<RenderScene_Material> standard_path_material_instances;

			std::map<GLuint, std::size_t> standard_path_material_indices;

			/**
			 * \brief Rendering commands for all lightmask volume objects.
			 */
			std::vector<RenderScene_Material> lightmask_volume_path_material_instances;

			/**
			 * \brief Mapping between objects and their lightmaps.
			 */
			std::map<NodeId, RenderScene_Lightmap> node_lightmaps;

			/* Lightmap data */
			Vec3 light_dir;
			color::RGBF32 light_intensity;
		};

		void RenderScene_render(
			const RenderScene_Commands& commands,
			const Mat4& view_matrix,
			const Mat4& proj_matrix);

		void RenderScene_update_matrices(
			RenderScene_Commands& commands,
			const NodeId* const nodes_ids,
			const Mat4* const matrices,
			const std::size_t num_nodes);

		void RenderScene_remove_commands(
			RenderScene_Commands& commands,
			const NodeId* node_ids,
			std::size_t num_nodes);

		void RenderScene_insert_static_mesh_commands(
			RenderScene_Commands& commands,
			RenderResource& resources,
			const Node* const* const nodes,
			const CStaticMesh* const* const static_meshes,
			const std::size_t num_static_meshes);
	}
}
