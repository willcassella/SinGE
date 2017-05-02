// RenderScene.cpp

#include <Resource/Misc/LightmaskVolume.h>
#include <Engine/Components/Display/CStaticMesh.h>
#include "../private/RenderScene.h"
#include "../private/RenderResource.h"

namespace sge
{
	namespace gl_render
	{
		static void render_lightmask_volumes(
			const RenderScene_Commands& commands,
			const RenderResource& resources,
			const Mat4& view_matrix,
			const Mat4& proj_matrix)
		{
			RenderCommand_bind_material(
				resources.lightmask_volume_material.program_id,
				resources.lightmask_volume_material.uniforms,
				resources.lightmask_volume_material.params,
				view_matrix,
				proj_matrix);

			for (const auto& lightmask_volume : commands.lightmask_volume_mesh_instances)
			{
				RenderCommand_render_meshes(
					resources.lightmask_volume_material.uniforms,
					lightmask_volume.volume_mesh,
					&lightmask_volume.mesh_instance,
					1);
			}
		}

		static void render_lightmask_objects(
			const RenderScene_LightmaskObject* lightmask_objects,
			const size_t num_lightmask_objects,
			const Mat4& view_matrix,
			const Mat4& proj_matrix)
		{
			for (size_t i = 0; i < num_lightmask_objects; ++i)
			{
				RenderCommand_bind_material(
					lightmask_objects[i].material.program_id,
					lightmask_objects[i].material.uniforms,
					lightmask_objects[i].material.params,
					view_matrix,
					proj_matrix);

				RenderCommand_render_meshes(
					lightmask_objects[i].material.uniforms,
					lightmask_objects[i].mesh,
					&lightmask_objects[i].mesh_instance,
					1);
			}
		}


		static void render_spotlight_shadowmaps(
			const RenderScene_Commands& commands)
		{
			glDepthMask(GL_TRUE);
			glCullFace(GL_FRONT);
			glStencilFunc(GL_ALWAYS, 0, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glDepthFunc(GL_LEQUAL);

			// Set rendering parameters
			for (const auto& spotlight : commands.spotlights)
			{
				RenderCommand_bind_framebuffer(
					spotlight.shadow_framebuffer,
					spotlight.shadow_width,
					spotlight.shadow_height);

				glClearDepth(1.f);
				glClear(GL_DEPTH_BUFFER_BIT);

				// Render normal material instances
				for (const auto& material_instance : commands.standard_path_material_instances)
				{
					RenderCommand_bind_material(
						material_instance.material.program_id,
						material_instance.material.uniforms,
						material_instance.material.params,
						spotlight.view_matrix,
						spotlight.proj_matrix);

					for (const auto& mesh : material_instance.mesh_instances)
					{
						RenderCommand_render_meshes(
							material_instance.material.uniforms,
							mesh.mesh_command,
							mesh.instance_commands.data(),
							mesh.instance_commands.size());
					}
				}

				// Render lightmask receivers
				render_lightmask_objects(
					commands.lightmask_receiver_mesh_instances.data(),
					commands.lightmask_receiver_mesh_instances.size(),
					spotlight.view_matrix,
					spotlight.proj_matrix);
			}
		}

		void RenderScene_render(
			const RenderScene_Commands& commands,
			const RenderResource& resources,
			const GLuint gbuffer,
			const GLuint gbuffer_width,
			const GLuint gbuffer_height,
			const Mat4& view_matrix,
			const Mat4& proj_matrix)
		{
			// Render spotlights
			render_spotlight_shadowmaps(commands);

			// Set standard rendering parameters
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glCullFace(GL_BACK);
			glDepthFunc(GL_LEQUAL);

			// Bind the gbuffer
			render_scene_prepare_gbuffer(gbuffer);
			glViewport(0, 0, gbuffer_width, gbuffer_height);

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

				for (const auto& mesh : material_instance.mesh_instances)
				{
					RenderCommand_render_meshes(
						material_instance.material.uniforms,
						mesh.mesh_command,
						mesh.instance_commands.data(),
						mesh.instance_commands.size());
				}
			}

			/*--- DRAW ONLY DEPTH BACKFACES, INCREMENT STENCIL WHERE DRAWN ---*/

			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDepthMask(GL_TRUE);
			glStencilFunc(GL_ALWAYS, 3, 0xFF);
			glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
			glCullFace(GL_FRONT);

			glStencilMask(1 << 0);
			render_lightmask_volumes(
				commands,
				resources,
				view_matrix,
				proj_matrix);

			glStencilMask(1 << 1);
			render_lightmask_objects(
				commands.lightmask_receiver_mesh_instances.data(),
				commands.lightmask_receiver_mesh_instances.size(),
				view_matrix,
				proj_matrix);

			/*--- DISABLE DEPTH DRAWING, FRONTFACES, CLEAR STENCIL WHERE DEPTH FAIL ---*/

			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDepthMask(GL_FALSE);
			glStencilFunc(GL_EQUAL, 3, 0xFF);
			glStencilOp(GL_KEEP, GL_ZERO, GL_KEEP);
			glCullFace(GL_BACK);
			glStencilMask(0xF);

			render_lightmask_volumes(
				commands,
				resources,
				view_matrix,
				proj_matrix);
			render_lightmask_objects(commands.lightmask_receiver_mesh_instances.data(),
				commands.lightmask_receiver_mesh_instances.size(),
				view_matrix,
				proj_matrix);

			/*--- RESET DEPTH ---*/

			glDepthMask(GL_TRUE);
			glClearDepth(0.f);
			glClear(GL_DEPTH_BUFFER_BIT);

			/*--- ENABLE COLOR AND DEPTH, GREATER DEPTH, FRONTFACES, DRAW ONLY WHERE NOT CLEARED ---*/

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
			glStencilFunc(GL_LEQUAL, 3, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glCullFace(GL_BACK);
			glDepthFunc(GL_GEQUAL);

			render_lightmask_volumes(
				commands,
				resources,
				view_matrix,
				proj_matrix);
			render_lightmask_objects(
				commands.lightmask_receiver_mesh_instances.data(),
				commands.lightmask_receiver_mesh_instances.size(),
				view_matrix,
				proj_matrix);
		}

		void RenderScene_update_matrices(
			RenderScene_Commands& commands,
			const NodeId* const node_ids,
			const Mat4* const matrices,
			const size_t num_nodes)
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
							if (mesh_node_ids[i] == node_ids[search_i])
							{
								mesh_instances[i].world_transform = matrices[search_i];
								break;
							}
						}
					}
				}
			}

			for (auto& volume_instance : commands.lightmask_volume_mesh_instances)
			{
				for (size_t search_i = 0; search_i < num_nodes; ++search_i)
				{
					if (node_ids[search_i] == volume_instance.node_id)
					{
						volume_instance.mesh_instance.world_transform = matrices[search_i];
						break;
					}
				}
			}

			for (auto& receiver_instance : commands.lightmask_receiver_mesh_instances)
			{
				for (std::size_t search_i = 0; search_i < num_nodes; ++search_i)
				{
					if (node_ids[search_i] == receiver_instance.node_id)
					{
						receiver_instance.mesh_instance.world_transform = matrices[search_i];
						break;
					}
				}
			}
		}

		static void remove_mesh_commands(
			RenderScene_Mesh& mesh_command_set,
			const NodeId* const SGE_RESTRICT target_node_ids,
			const size_t num_target_node_ids)
		{
			size_t num_mesh_commands = mesh_command_set.instance_commands.size();
			auto* const mesh_commands = mesh_command_set.instance_commands.data();
			auto* const node_ids = mesh_command_set.node_ids.data();

			// For each mesh command
			for (size_t command_i = 0; command_i < num_mesh_commands;)
			{
				const NodeId node_id = node_ids[command_i];

				// See if the node associated with this mesh command appears in the list to be deleted
				size_t search_i = 0;
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
			const size_t num_nodes)
		{
			for (size_t i = 0; i < num_nodes; ++i)
			{
				const Node* const node = nodes[i];
				const CStaticMesh* const static_mesh = static_meshes[i];

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

				// If it's a a lightmask object, add it to the path for that
				if (static_mesh->lightmask_mode() != CStaticMesh::LightmaskMode::NONE)
				{
					// Create the command
					RenderScene_LightmaskObject command;
					command.node_id = node->get_id();
					command.material = material_resource;
					command.mesh.vao = mesh_resource.vao;
					command.mesh.start_element_index = 0;
					command.mesh.num_element_indices = mesh_resource.num_total_elements;
					command.mesh.base_vertex = 0;
					command.mesh_instance.world_transform = node->get_world_matrix();
					command.mesh_instance.mat_uv_scale = static_mesh->uv_scale();
					command.mesh_instance.lightmap_x_basis = lightmap.x_basis_tex;
					command.mesh_instance.lightmap_y_basis = lightmap.y_basis_tex;
					command.mesh_instance.lightmap_z_basis = lightmap.z_basis_tex;
					command.mesh_instance.lightmap_direct_mask = lightmap.direct_mask_tex;

					if (static_mesh->lightmask_mode() == CStaticMesh::LightmaskMode::OCCLUDER)
					{
						commands.lightmask_occluder_mesh_instances.push_back(std::move(command));
					}
					else
					{
						commands.lightmask_receiver_mesh_instances.push_back(std::move(command));
					}
					continue;
				}

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

		void RenderScene_remove_static_mesh_commands(
			RenderScene_Commands& commands,
			const NodeId* const target_node_ids,
			const size_t num_target_node_ids)
		{
			// Check standard path instances
			for (auto& material_instance : commands.standard_path_material_instances)
			{
				for (auto& mesh : material_instance.mesh_instances)
				{
					remove_mesh_commands(mesh, target_node_ids, num_target_node_ids);
				}
			}

			// Check lightmask receiver instances
			size_t num_lightmask_receivers = commands.lightmask_receiver_mesh_instances.size();
			auto* const lightmask_receivers = commands.lightmask_receiver_mesh_instances.data();
			for (size_t i = 0; i < num_lightmask_receivers;)
			{
				int incr = 1;
				for (size_t search_i = 0; search_i < num_target_node_ids; ++search_i)
				{
					if (target_node_ids[search_i] == lightmask_receivers[i].node_id)
					{
						num_lightmask_receivers -= 1;
						lightmask_receivers[search_i] = std::move(lightmask_receivers[num_lightmask_receivers]);
						incr = 0;
						break;
					}
				}

				i += incr;
			}

			commands.lightmask_receiver_mesh_instances.resize(num_lightmask_receivers);
		}

		void RenderScene_insert_spotlight_commands(
			RenderScene_Commands& commands,
			RenderResource& resources,
			const Node* const* const nodes,
			const CSpotlight* const* const spotlights,
			const size_t num_spotlights)
		{
			for (size_t i = 0; i < num_spotlights; ++i)
			{
				if (spotlights[i]->casts_shadows())
				{
					// Create a shadow map object
					RenderScene_Spotlight spotlight_shadow_map;
					spotlight_shadow_map.node_id = nodes[i]->get_id();
					spotlight_shadow_map.view_matrix = nodes[i]->get_world_matrix().inverse();
					spotlight_shadow_map.proj_matrix = Mat4::perspective_projection(
						spotlights[i]->frustum_horiz_angle(),
						spotlights[i]->frustum_vert_angle(),
						spotlights[i]->near_clipping_plane(),
						spotlights[i]->far_clipping_plane());
					spotlight_shadow_map.shadow_width = spotlights[i]->shadow_map_width(),
					spotlight_shadow_map.shadow_height = spotlights[i]->shadow_map_height();

					GLuint shadow_depth_map;
					glGenTextures(1, &shadow_depth_map);
					glBindTexture(GL_TEXTURE_2D, shadow_depth_map);
					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_DEPTH_COMPONENT,
						spotlight_shadow_map.shadow_width,
						spotlight_shadow_map.shadow_height,
						0,
						GL_DEPTH_COMPONENT,
						GL_FLOAT,
						nullptr);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

					// Create the framebuffer for the shadow map
					GLuint shadow_framebuffer;
					glGenFramebuffers(1, &shadow_framebuffer);
					glBindFramebuffer(GL_FRAMEBUFFER, shadow_framebuffer);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_depth_map, 0);
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);

					spotlight_shadow_map.shadow_framebuffer = shadow_framebuffer;
					spotlight_shadow_map.shadow_depthbuffer = shadow_depth_map;
					commands.spotlights.push_back(spotlight_shadow_map);
				}

				if (!spotlights[i]->is_lightmask_volume() || spotlights[i]->shape() != CSpotlight::Shape::FRUSTUM)
				{
					continue;
				}

				// Create the VAO for this volume
				GLuint volume_vao;
				glGenVertexArrays(1, &volume_vao);
				glBindVertexArray(volume_vao);

				// Bind the EBO for this mesh
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resources.frustum_ebo);

				// Create the frustum for this volume
				Vec3 frustum_vert_positions[NUM_FRUSTUM_VERTS];
				Vec3 frustum_vert_normals[NUM_FRUSTUM_VERTS];
				Vec2 frustum_vert_texcoords[NUM_FRUSTUM_VERTS];
				create_lightmask_volume_frustum_positions(
					spotlights[i]->near_clipping_plane(),
					spotlights[i]->far_clipping_plane(),
					spotlights[i]->frustum_horiz_angle().radians(),
					spotlights[i]->frustum_vert_angle().radians(),
					frustum_vert_positions);
				create_lightmask_volume_frustum_normals(
					spotlights[i]->frustum_horiz_angle().radians(),
					spotlights[i]->frustum_vert_angle().radians(),
					frustum_vert_normals);
				create_lightmask_volume_frustum_texcoords(
					spotlights[i]->near_clipping_plane(),
					spotlights[i]->far_clipping_plane(),
					spotlights[i]->frustum_horiz_angle().radians(),
					spotlights[i]->frustum_vert_angle().radians(),
					frustum_vert_texcoords);

				// Upload data and set vertex position attribute
				GLuint volume_pos_buffer;
				glGenBuffers(1, &volume_pos_buffer);
				glBindBuffer(GL_ARRAY_BUFFER, volume_pos_buffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(frustum_vert_positions), frustum_vert_positions, GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(gl_material::POSITION_ATTRIB_LOCATION);
				glVertexAttribPointer(gl_material::POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), nullptr);

				// Upload data and set vertex normal attribute
				GLuint volume_normal_buffer;
				glGenBuffers(1, &volume_normal_buffer);
				glBindBuffer(GL_ARRAY_BUFFER, volume_normal_buffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(frustum_vert_normals), frustum_vert_normals, GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(gl_material::NORMAL_ATTRIB_LOCATION);
				glVertexAttribPointer(gl_material::NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), nullptr);

				// Upload data and set vertex texcoord attribute
				GLuint volume_texcoord_buffer;
				glGenBuffers(1, &volume_texcoord_buffer);
				glBindBuffer(GL_ARRAY_BUFFER, volume_texcoord_buffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(frustum_vert_texcoords), frustum_vert_texcoords, GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(gl_material::MATERIAL_TEXCOORD_ATTRIB_LOCATION);
				glVertexAttribPointer(gl_material::MATERIAL_TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), nullptr);

				// Create a LightmaskVolume command
				RenderScene_LightmaskVolume volume_command;
				volume_command.node_id = nodes[i]->get_id();
				volume_command.pos_buffer = volume_pos_buffer;
				volume_command.texcoord_buffer = volume_texcoord_buffer;
				volume_command.volume_mesh.vao = volume_vao;
				volume_command.volume_mesh.start_element_index = 0;
				volume_command.volume_mesh.num_element_indices = NUM_FRUSTUM_ELEMS;
				volume_command.volume_mesh.base_vertex = 0;
				volume_command.mesh_instance.world_transform = nodes[i]->get_world_matrix();
				volume_command.mesh_instance.mat_uv_scale = Vec2{ 1.f, 1.f };
				volume_command.mesh_instance.lightmap_x_basis = 0;
				volume_command.mesh_instance.lightmap_y_basis = 0;
				volume_command.mesh_instance.lightmap_z_basis = 0;
				volume_command.mesh_instance.lightmap_direct_mask = 0;

				// Add it to the command buffer
				commands.lightmask_volume_mesh_instances.push_back(volume_command);
				glBindVertexArray(0);
			}
		}

		void RenderScene_update_spotlight_commands(
			RenderScene_Commands& commands,
			RenderResource& resources,
			const Node* const* const nodes,
			const CSpotlight* const* const spotlights,
			const size_t num_spotlights)
		{
			for (size_t i = 0; i < num_spotlights; ++i)
			{
				const auto num_lightmasks = commands.lightmask_volume_mesh_instances.size();
				auto* const lightmasks = commands.lightmask_volume_mesh_instances.data();

				// Search for the spotlight in the array
				bool found = false;
				for (size_t lightmask_i = 0; lightmask_i < num_lightmasks; ++lightmask_i)
				{
					if (lightmasks[lightmask_i].node_id != spotlights[i]->node())
					{
						continue;
					}

					found = true;

					// Check if they've disabled lightmasking on this spotlight
					if (!spotlights[i]->is_lightmask_volume() || spotlights[i]->shape() != CSpotlight::Shape::FRUSTUM)
					{
						// Remove it
						lightmasks[lightmask_i] = lightmasks[num_lightmasks - 1];
						commands.lightmask_volume_mesh_instances.resize(num_lightmasks - 1);
						break;
					}

					// Update shape of this lightmask
					Vec3 frustum_positions[NUM_FRUSTUM_VERTS];
					Vec3 frustum_normals[NUM_FRUSTUM_VERTS];
					Vec2 frustum_texcoords[NUM_FRUSTUM_VERTS];
					create_lightmask_volume_frustum_positions(
						spotlights[i]->near_clipping_plane(),
						spotlights[i]->far_clipping_plane(),
						spotlights[i]->frustum_horiz_angle().radians(),
						spotlights[i]->frustum_vert_angle().radians(),
						frustum_positions);
					create_lightmask_volume_frustum_normals(
						spotlights[i]->frustum_horiz_angle().radians(),
						spotlights[i]->frustum_vert_angle().radians(),
						frustum_normals);
					create_lightmask_volume_frustum_texcoords(
						spotlights[i]->near_clipping_plane(),
						spotlights[i]->far_clipping_plane(),
						spotlights[i]->frustum_horiz_angle().radians(),
						spotlights[i]->frustum_vert_angle().radians(),
						frustum_texcoords);

					// Upload data
					glBindBuffer(GL_ARRAY_BUFFER, lightmasks[lightmask_i].pos_buffer);
					glBufferData(GL_ARRAY_BUFFER, sizeof(frustum_positions), frustum_positions, GL_DYNAMIC_DRAW);
					glBindBuffer(GL_ARRAY_BUFFER, lightmasks[lightmask_i].normal_buffer);
					glBufferData(GL_ARRAY_BUFFER, sizeof(frustum_normals), frustum_normals, GL_DYNAMIC_DRAW);
					glBindBuffer(GL_ARRAY_BUFFER, lightmasks[lightmask_i].texcoord_buffer);
					glBufferData(GL_ARRAY_BUFFER, sizeof(frustum_texcoords), frustum_texcoords, GL_DYNAMIC_DRAW);
					break;
				}

				// If this spotlight wasn't already a lightmask and lightmasking was enabled, add it to the array
				if (!found)
				{
					RenderScene_insert_spotlight_commands(commands, resources, nodes + i, spotlights + i, 1);
				}
			}
		}

		void RenderScene_remove_spotlight_commands(
			RenderScene_Commands& commands,
			NodeId* const node_ids,
			size_t num_nodes)
		{
			size_t num_lightmasks = commands.lightmask_volume_mesh_instances.size();
			auto* const lightmasks = commands.lightmask_volume_mesh_instances.data();

			for (size_t i = 0; i < num_lightmasks;)
			{
				int incr = 1;

				for (size_t search_i = 0; search_i < num_nodes; ++search_i)
				{
					if (lightmasks[i].node_id != node_ids[search_i])
					{
						continue;
					}

					// Remove the spotlight command, and the node id
					num_lightmasks -= 1;
					num_nodes -= 1;
					lightmasks[i] = lightmasks[num_lightmasks];
					node_ids[search_i] = node_ids[num_nodes];
					incr = 0;
					break;
				}

				i += incr;
			}

			commands.lightmask_volume_mesh_instances.resize(num_lightmasks);
		}

		void RenderScene_clear(
			RenderScene_Commands& commands)
		{
			commands.lightmask_occluder_mesh_instances.clear();
			commands.lightmask_receiver_mesh_instances.clear();
			commands.lightmask_volume_mesh_instances.clear();
			commands.standard_path_material_indices.clear();
			commands.standard_path_material_instances.clear();
			commands.spotlights.clear();

			for (auto node : commands.node_lightmaps)
			{
				GLuint textures[] = {
					node.second.x_basis_tex,
					node.second.y_basis_tex,
					node.second.z_basis_tex,
					node.second.direct_mask_tex
				};
				glDeleteTextures(4, textures);
			}

			commands.node_lightmaps.clear();
		}
	}
}
