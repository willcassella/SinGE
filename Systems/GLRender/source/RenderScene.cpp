// RenderScene.cpp

#include <Engine/Components/Display/CStaticMesh.h>
#include "../private/RenderScene.h"
#include "../private/RenderResource.h"

namespace sge
{
	namespace gl_render
	{
		void RenderScene_render(
			const RenderScene_Commands& commands,
			const Mat4& view_matrix,
			const Mat4& proj_matrix)
		{
			// Render standard material instances
			for (const auto& material_instance : commands.standard_path_material_instances)
			{
				// Bind the material
				RenderCommand_bind_material(
					material_instance.material.program_id,
					material_instance.material.uniforms,
					material_instance.material.params,
					view_matrix,
					proj_matrix);

				// Bind uniforms
				GLenum next_active_texture = gl_material::FIRST_USER_TEXTURE_SLOT;
				gl_material::set_bound_material_params(&next_active_texture, material_instance.material.params);

				for (const auto& mesh : material_instance.mesh_instances)
				{
					RenderCommand_render_meshes(
						material_instance.material.uniforms,
						mesh.mesh_command,
						mesh.instance_commands.data(),
						mesh.instance_commands.size());
				}
			}
		}

		void RenderScene_update_matrices(
			RenderScene_Commands& commands,
			const NodeId* const node_ids,
			const Mat4* const matrices,
			const std::size_t num_nodes)
		{
			for (auto& material_instance : commands.standard_path_material_instances)
			{
				for (auto& mesh : material_instance.mesh_instances)
				{
					const auto num_instances = mesh.node_ids.size();
					const auto* const mesh_node_ids = mesh.node_ids.data();
					auto* const mesh_instances = mesh.instance_commands.data();

					for (std::size_t i = 0; i < num_instances; ++i)
					{
						for (std::size_t search_i = 0; search_i < num_nodes; ++search_i)
						{
							if (mesh_node_ids[i] != node_ids[search_i])
							{
								continue;
							}

							mesh_instances[i].world_transform = matrices[search_i];
						}
					}
				}
			}

			for (auto& material_instance : commands.lightmask_volume_path_material_instances)
			{
				for (auto& mesh : material_instance.mesh_instances)
				{
					const auto num_instances = mesh.node_ids.size();
					const auto* const volume_node_ids = mesh.node_ids.data();
					auto* const mesh_instances = mesh.instance_commands.data();

					for (std::size_t i = 0; i < num_instances; ++i)
					{
						for (std::size_t search_i = 0; search_i < num_nodes; ++search_i)
						{
							if (node_ids[search_i] != volume_node_ids[i])
							{
								continue;
							}

							mesh_instances[i].world_transform = matrices[search_i];
						}
					}
				}
			}
		}

		static void remove_mesh_commands(
			RenderScene_Mesh& mesh_command_set,
			const NodeId* SGE_RESTRICT target_node_ids,
			std::size_t num_target_node_ids)
		{
			std::size_t num_mesh_commands = mesh_command_set.instance_commands.size();
			auto* const mesh_commands = mesh_command_set.instance_commands.data();
			auto* const node_ids = mesh_command_set.node_ids.data();

			// For each mesh command
			for (std::size_t command_i = 0; command_i < num_mesh_commands;)
			{
				const NodeId node_id = node_ids[command_i];

				// See if the node associated with this mesh command appears in the list to be deleted
				std::size_t search_i = 0;
				for (; search_i < num_target_node_ids; ++search_i)
				{
					if (node_id == target_node_ids[search_i])
					{
						break;
					}
				}

				// If this command is not to be removed, jump to next command
				if (search_i == num_target_node_ids)
				{
					command_i += 1;
					continue;
				}

				// Remove this instance
				num_mesh_commands -= 1;
				if (command_i == num_mesh_commands)
				{
					break;
				}

				// Copy the last command into this position (don't increment iterator)
				mesh_commands[command_i] = mesh_commands[num_mesh_commands];
				node_ids[command_i] = node_ids[num_mesh_commands];
			}

			// Fix up command size
			mesh_command_set.instance_commands.resize(num_mesh_commands);
			mesh_command_set.node_ids.resize(num_mesh_commands);
		}

		void RenderScene_remove_commands(
			RenderScene_Commands& commands,
			const NodeId* target_node_ids,
			std::size_t num_target_node_ids)
		{
			for (auto& material_instance : commands.standard_path_material_instances)
			{
				for (auto& mesh : material_instance.mesh_instances)
				{
					remove_mesh_commands(mesh, target_node_ids, num_target_node_ids);
				}
			}

			for (auto& material_instance : commands.lightmask_volume_path_material_instances)
			{
				for (auto& mesh_command_set : material_instance.mesh_instances)
				{
					remove_mesh_commands(mesh_command_set, target_node_ids, num_target_node_ids);
				}
			}
		}

		RenderScene_Lightmap get_lightmap(
			const RenderScene_Commands& commands,
			const NodeId node_id)
		{
			const auto lightmap_iter = commands.node_lightmaps.find(node_id);
			return lightmap_iter != commands.node_lightmaps.end() ? lightmap_iter->second : RenderScene_Lightmap{};
		}

		void RenderScene_insert_static_mesh_commands(
			RenderScene_Commands& commands,
			RenderResource& resources,
			const Node* const* const nodes,
			const CStaticMesh* const* const static_meshes,
			const std::size_t num_nodes)
		{
			for (std::size_t i = 0; i < num_nodes; ++i)
			{
				const Node* const node = nodes[i];
				const CStaticMesh* const static_mesh = static_meshes[i];

				// Don't handle non-standard rendering paths yet
				if (static_mesh->lightmask_mode() != CStaticMesh::LightmaskMode::NONE)
				{
					continue;
				}

				// Get the static mesh resource for this instance
				const auto& mesh_resource = RenderResource_get_static_mesh_resource(
					resources,
					static_mesh->mesh().c_str());

				// Get the material resource for this instance
				const auto& material_resource = RenderResource_get_material_resource(
					resources,
					static_mesh->material().c_str());

				// Get the lightmap for this instance
				const auto lightmap = get_lightmap(commands, node->get_id());

				// Search for the material
				const auto material_iter = commands.standard_path_material_indices.find(material_resource.program_id);
				if (material_iter == commands.standard_path_material_indices.end())
				{
					commands.standard_path_material_indices.insert(std::make_pair(
						material_resource.program_id,
						commands.standard_path_material_instances.size()));

					// Create the Material object
					RenderScene_Material mat_instance;
					mat_instance.material = material_resource;

					// Create the mesh command set object
					RenderScene_Mesh mesh_command_set;
					mesh_command_set.mesh_command.vao = mesh_resource.vao;
					mesh_command_set.mesh_command.start_element_index = 0;
					mesh_command_set.mesh_command.num_element_indices = mesh_resource.num_total_elements;
					mesh_command_set.mesh_command.base_vertex = 0;

					// Create the mesh instance object
					RenderCommand_MeshInstance instance;
					instance.world_transform = node->get_world_matrix();
					instance.mat_uv_scale = static_mesh->uv_scale();
					instance.lightmap_x_basis = lightmap.x_basis_tex;
					instance.lightmap_y_basis = lightmap.y_basis_tex;
					instance.lightmap_z_basis = lightmap.z_basis_tex;
					instance.lightmap_direct_mask = lightmap.direct_mask_tex;

					// Insert it into the command set
					mesh_command_set.instance_commands.push_back(instance);
					mesh_command_set.node_ids.push_back(node->get_id());
					mat_instance.mesh_instances.push_back(std::move(mesh_command_set));
					mat_instance.mesh_indices.insert(std::make_pair(mesh_resource.vao, 0));
					commands.standard_path_material_instances.push_back(std::move(mat_instance));
					continue;
				}

				// Search for the mesh in the material
				const auto material_index = material_iter->second;
				auto& material = commands.standard_path_material_instances[material_index];
				auto mesh_iter = material.mesh_indices.find(mesh_resource.vao);
				if (mesh_iter == material.mesh_indices.end())
				{
					material.mesh_indices.insert(std::make_pair(
						mesh_resource.vao,
						material.mesh_instances.size()));

					// Create the mesh command set object
					RenderScene_Mesh mesh_command_set;
					mesh_command_set.mesh_command.vao = mesh_resource.vao;
					mesh_command_set.mesh_command.start_element_index = 0;
					mesh_command_set.mesh_command.num_element_indices = mesh_resource.num_total_elements;
					mesh_command_set.mesh_command.base_vertex = 0;

					// Create the mesh instance object
					RenderCommand_MeshInstance instance;
					instance.world_transform = node->get_world_matrix();
					instance.mat_uv_scale = static_mesh->uv_scale();
					instance.lightmap_x_basis = lightmap.x_basis_tex;
					instance.lightmap_y_basis = lightmap.y_basis_tex;
					instance.lightmap_z_basis = lightmap.z_basis_tex;
					instance.lightmap_direct_mask = lightmap.direct_mask_tex;

					// Insert it into the command set
					mesh_command_set.instance_commands.push_back(instance);
					mesh_command_set.node_ids.push_back(node->get_id());
					material.mesh_instances.push_back(std::move(mesh_command_set));
					continue;
				}

				// Creat the mesh instance object
				RenderCommand_MeshInstance instance;
				instance.world_transform = node->get_world_matrix();
				instance.mat_uv_scale = static_mesh->uv_scale();
				instance.lightmap_x_basis = lightmap.x_basis_tex;
				instance.lightmap_y_basis = lightmap.y_basis_tex;
				instance.lightmap_z_basis = lightmap.z_basis_tex;
				instance.lightmap_direct_mask = lightmap.direct_mask_tex;

				// Insert it into the command set
				material.mesh_instances[mesh_iter->second].instance_commands.push_back(instance);
			}
		}
	}
}
