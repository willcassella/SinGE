// EditorOps.cpp

#include <iostream>
#include <chrono>
#include <future>
#include <Core/Memory/Functions.h>
#include <Core/IO/ArchiveWriter.h>
#include <Core/IO/ArchiveReader.h>
#include <Core/Reflection/Reflection.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/Reflection/TypeDB.h>
#include <Core/Reflection/PropertyInfo.h>
#include <Core/Interfaces/IToString.h>
#include <Resource/Interfaces/IFromFile.h>
#include <Resource/Archives/JsonArchive.h>
#include <Resource/Archives/BinaryArchive.h>
#include <Resource/Resources/StaticMesh.h>
#include <Lightmapper/Lightmapper.h>
#include <Engine/Scene.h>
#include <Engine/Components/Display/CStaticMesh.h>

namespace sge
{
	namespace editor_server
	{
		namespace ops
		{
			static void read_type_info(const TypeInfo& type, ArchiveWriter& writer)
			{
				// Serialize the properties
				writer.push_object_member("properties");
				type.enumerate_properties([&writer](const char* propName, const PropertyInfo& propInfo)
				{
					// Don't send property info for hidden properties, or read-only, or containers
					if (propInfo.is_read_only() || propInfo.flags() & PF_EDITOR_HIDDEN || propInfo.type().flags() & TF_CONTAINER)
					{
						return;
					}

					// Serialize the property name
					writer.push_object_member(propName);

					// Write property information
					writer.object_member("index", propInfo.index());
					writer.object_member("type", propInfo.type().name());
					writer.object_member("category", propInfo.category());
					writer.object_member("description", propInfo.description());

					// Write property attributes
					writer.push_object_member("attributes");
					if (propInfo.flags() & PF_EDITOR_DEFAULT_COLLAPSED)
					{
						writer.array_element("default_collapsed");
					}
					writer.pop(); // "attributes"

					writer.pop(); // propName
				});
				writer.pop();
			}

			static void read_properties(Any<> object, ArchiveWriter& writer)
			{
				object.type().enumerate_properties([object, &writer](const char* propName, const PropertyInfo& propInfo)
				{
					if (propInfo.flags() & (PF_EDITOR_HIDDEN | PF_EDITOR_DEFAULT_COLLAPSED))
					{
						return;
					}
					if (propInfo.is_read_only())
					{
						return;
					}
					if (propInfo.type().flags() & TF_CONTAINER)
					{
						return;
					}

					writer.push_object_member(propName);

					// Access the property
					propInfo.get(object.object(), [&writer](Any<> prop)
					{
						// If the property's type is a terminal type
						if (prop.type().flags() & TF_RECURSE_TERMINAL)
						{
							// Get the IToArchive implementation
							const auto* const impl = sge::get_vtable<IToArchive>(prop.type());
							if (!impl)
							{
								return;
							}

							impl->to_archive(prop.object(), writer);
							return;
						}

						read_properties(prop, writer);
					});

					writer.pop(); // propName
				});
			}

			static void write_properties(AnyMut<> object, ArchiveReader& reader, ArchiveWriter& writer)
			{
				reader.enumerate_object_members([object, &reader, &writer](const char* propName)
				{
					// Search for the property
					const auto* propInfo = object.type().find_property(propName);
					if (!propInfo)
					{
						return;
					}

					// Modify the property
					propInfo->mutate(object.object(), [propName, &reader, &writer](AnyMut<> prop)
					{
						// If we need to recurse deeper
						if ((prop.type().flags() & TF_RECURSE_TERMINAL) == 0)
						{
							writer.push_object_member(propName);
							write_properties(prop, reader, writer);
							writer.pop(); // propName
							return;
						}

						// Get the FromArchive and ToArchive impls for this type
						const auto* const from_archive_impl = sge::get_vtable<IFromArchive>(prop.type());
						const auto* const to_archive_impl = sge::get_vtable<IToArchive>(prop.type());
						if (!from_archive_impl || !to_archive_impl)
						{
							return;
						}

						// Update the property
						from_archive_impl->from_archive(prop.object(), reader);

						// Output the new value
						writer.push_object_member(propName);
						to_archive_impl->to_archive(prop.object(), writer);
						writer.pop(); // propName
					});
				});
			}

			void get_type_info_query(const TypeDB& type_db, ArchiveReader& reader, ArchiveWriter& writer)
			{
				// Enumerate the types to get the properties from
				reader.enumerate_array_elements([&type_db, &reader, &writer](std::size_t /*i*/)
				{
					// Get the type name
					std::string typeName;
					sge::from_archive(typeName, reader);

					// Search for the type
					const auto* type = type_db.find_type(typeName.c_str());
					if (!type)
					{
						std::cout << "Error: Type '" << typeName << "' was not found in type database." << std::endl;
						return;
					}

					std::cout << "Sending type info for " << typeName << std::endl;
					writer.push_object_member(typeName.c_str());
					read_type_info(*type, writer);
					writer.pop();
				});
			}

			void get_component_types_query(const Scene& scene, ArchiveWriter& writer)
			{
				std::cout << "Sending component list" << std::endl;

				for (const auto& component_type : scene.get_raw_scene_data().components)
				{
					writer.array_element(component_type.first->name());
				}
			}

			void new_node_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
			{
				reader.enumerate_object_members([&scene, &reader, &writer](const char* fake_id_str)
				{
					// Create the node
					Node* node = nullptr;
					scene.create_nodes(1, &node);

					// Set the properties on the node
					std::string name;
					reader.object_member("name", name);
					node->set_name(std::move(name));

					// Output results on the writer
					writer.push_object_member(fake_id_str);
					writer.object_member("id", node->get_id());
					writer.object_member("name", node->get_name());
					writer.pop(); // fake_id_str

					std::cout << "Created new node: " << node->get_id().to_u64() << std::endl;
				});
			}

			void destroy_node_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
			{
				reader.enumerate_array_elements([&scene, &reader, &writer](std::size_t /*i*/)
				{
					// Get the id of the node to destroy
					NodeId node_id;
					sge::from_archive(node_id, reader);

					// Get the node
					Node* node = nullptr;
					scene.get_nodes(&node_id, 1, &node);
					if (!node)
					{
						return;
					}

					// Destroy it
					std::cout << "Destroying node " << node_id.to_u64() << std::endl;
                    scene.destroy_nodes(1, &node);

					// Add it to the writer
					writer.array_element(node_id);
				});
			}

			void node_name_update_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
			{
				reader.enumerate_object_members([&scene, &reader, &writer](const char* node_id_str)
				{
					// Get the node to set the name of
					NodeId node_id;
					node_id.from_string(node_id_str);

					// Get the name to set
					std::string name;
					sge::from_archive(name, reader);

					// Get the node
					Node* node;
					scene.get_nodes(&node_id, 1, &node);
					if (!node)
					{
						return;
					}

					// Set the name
					std::cout << "Setting name on node " << node_id_str << " to '" << name << "'" << std::endl;
					node->set_name(std::move(name));

					// Output on the writer
					writer.object_member(node_id_str, node->get_name());
				});
			}

			void node_root_update_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
			{
				reader.enumerate_object_members([&scene, &reader, &writer](const char* node_id_str)
				{
					// Get the node to set the parent of
					NodeId node_id;
					Node* node = nullptr;
					node_id.from_string(node_id_str);
					scene.get_nodes(&node_id, 1, &node);
					if (!node)
					{
						std::cout << "Error: Invalid node id to set root of." << std::endl;
						return;
					}

					// Get the node to set as the root
					NodeId root_id;
					Node* root = nullptr;
					root_id.from_archive(reader);
					scene.get_nodes(&root_id, 1, &root);
					if (!root && !root_id.is_null())
					{
						std::cout << "Error: Invalid node id to set as root." << std::endl;
						return;
					}

					// Set the root
					std::cout << "Setting root on node " << node_id_str << " to " << root_id.to_u64() << std::endl;
					node->set_root(root);

					// Output on the writer
					writer.object_member(node_id_str, root_id);
				});
			}

			void node_local_transform_update_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
			{
				reader.enumerate_object_members([&scene, &reader, &writer](const char* node_id_str)
				{
					// Get the node to set the transform of
					NodeId node_id;
					Node* node = nullptr;
					node_id.from_string(node_id_str);
					scene.get_nodes(&node_id, 1, &node);
					if (!node)
					{
						std::cout << "Error: Invalid node id to set local transform." << std::endl;
						return;
					}

					std::cout << "Setting local transform on node " << node_id_str << std::endl;

					// Update the writer
					writer.push_object_member(node_id_str);

					// Set the transform
					Vec3 local_pos = node->get_local_position();
					reader.object_member("lpos", local_pos);
					writer.object_member("lpos", local_pos);
					node->set_local_position(local_pos);

					Vec3 local_scale = node->get_local_scale();
					reader.object_member("lscale", local_scale);
					writer.object_member("lscale", local_scale);
					node->set_local_scale(local_scale);

					Quat local_rot = node->get_local_rotation();
					reader.object_member("lrot", local_rot);
					writer.object_member("lrot", local_rot);
					node->set_local_rotation(local_rot);

					writer.pop(); // node_id_str
				});
			}

			void new_component_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
			{
				// Enumerate components to add nodes to
				reader.enumerate_object_members([&scene, &reader, &writer](const char* component_type_name)
				{
					// Search for the component type name
					const auto* const type = scene.get_component_type(component_type_name);
					if (!type)
					{
						std::cout << "Error: Invalid component type name: '" << component_type_name << "'" << std::endl;
						return;
					}

					// Get the component container
					auto* const container = scene.get_component_container(*type);
					if (!container)
					{
						std::cout << "Error: Invalid component type: '" << component_type_name << "'" << std::endl;
						return;
					}

					writer.push_object_member(component_type_name);

					// Enumerate nodes to add
					reader.enumerate_array_elements([type, container, &scene, &reader, &writer](std::size_t /*i*/)
					{
						NodeId node_id;
						node_id.from_archive(reader);

						Node* node;
						scene.get_nodes(&node_id, 1, &node);

						// Create the component
						void* instance = nullptr;
						container->create_instances(&node, 1, &instance);
						std::cout << "Created '" << type->name() << "' component on node " << node_id.to_u64() << std::endl;

						// Output values
						char node_id_str[20];
						node_id.to_string(node_id_str, 20);
						writer.push_object_member(node_id_str);
						read_properties(Any<>{ *type, instance }, writer);
						writer.pop(); // node_id_str
					});

					writer.pop(); // component_type_name
				});
			}

			void destroy_component_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
			{
				// Enumerate component types to destroy
				reader.enumerate_object_members([&scene, &reader, &writer](const char* component_type_name)
				{
					// Get the component type
					const auto* const type = scene.get_component_type(component_type_name);
					if (!type)
					{
						std::cout << "Error: Invalid component type name '" << component_type_name << "'" << std::endl;
						return;
					}

					// Get the container
					auto* const container = scene.get_component_container(*type);
					if (!container)
					{
						std::cout << "Error: Invalid component type '" << component_type_name << "'" << std::endl;
						return;
					}

					writer.push_object_member(component_type_name);

					// Enumerate instances to destroy
					reader.enumerate_array_elements([component_type_name, container, &scene, &reader, &writer](std::size_t /*i*/)
					{
						// Get the node id
						NodeId node;
						node.from_archive(reader);

						// Destroy it
						container->remove_instances(&node, 1);
						std::cout << "Destroyed '" << component_type_name << "' component on node " << node.to_u64() << std::endl;

						// Output to the writer
						writer.array_element(node);
					});

					writer.pop(); // component_type_name
				});
			}

			void get_scene_query(Scene& scene, ArchiveWriter& writer)
			{
				const auto& scene_data = scene.get_raw_scene_data();
				std::cout << "Sending scene information" << std::endl;

				writer.as_object();

				// Iterate over nodes
				writer.push_object_member("nodes");
				writer.as_object();
				for (auto node : scene_data.nodes)
				{
					char node_id_str[20];
					node.first.to_string(node_id_str, 20);

					writer.push_object_member(node_id_str);
					writer.as_object();
					writer.object_member("name", node.second->get_name());
					writer.object_member("root", node.second->get_root());
					writer.object_member("lpos", node.second->get_local_position());
					writer.object_member("lscale", node.second->get_local_scale());
					writer.object_member("lrot", node.second->get_local_rotation());
					writer.pop(); // node_id_str
				}
				writer.pop(); // "nodes"

				// Iterate over components
				writer.push_object_member("components");
				writer.as_object();
				for (const auto& component_type : scene_data.components)
				{
					NodeId instance_nodes[8];
					void* instances[8];
					std::size_t num_nodes = 0;
					std::size_t start_index = 0;

					writer.push_object_member(component_type.first->name().c_str());

					// Get instances of this component
					while (component_type.second->get_instance_nodes(start_index, 8, &num_nodes, instance_nodes))
					{
						start_index += 8;
						component_type.second->get_instances(instance_nodes, num_nodes, instances);

						// For each instance
						for (std::size_t i = 0; i < num_nodes; ++i)
						{
							char node_id_str[20];
							instance_nodes[i].to_string(node_id_str, 20);

							writer.push_object_member(node_id_str);
							read_properties(Any<>{ *component_type.first, instances[i] }, writer);
							writer.pop(); // node_id_str
						}
					}

				    writer.pop(); // type name
				}
				writer.pop(); // "components"
			}

			void component_property_update_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
			{
				// Enumerate types of components changed
				reader.enumerate_object_members([&scene, &reader, &writer](const char* component_type_name)
				{
					const auto* const type = scene.get_component_type(component_type_name);
					if (!type)
					{
						std::cout << "Invalid component type name: '" << component_type_name << "'" << std::endl;
						return;
					}

					auto* const container = scene.get_component_container(*type);
					if (!container)
					{
						std::cout << "Invalid component type: '" << component_type_name << "'" << std::endl;
						return;
					}

					writer.push_object_member(component_type_name);

					// Enumerate the node Ids of changed components
					reader.enumerate_object_members([type, container, &scene, &reader, &writer](const char* node_id_str)
					{
						NodeId node_id;
						node_id.from_string(node_id_str);

					    // Retrieve the component instance
						void* component;
						container->get_instances(&node_id, 1, &component);
						if (!component)
						{
							return;
						}

						// Deserialize it
						std::cout << "Writing properties of '" << type->name() << "' component on node '" << node_id_str << "'" << std::endl;
						writer.push_object_member(node_id_str);
						write_properties(AnyMut<>{ *type, component }, reader, writer);
						writer.pop(); // node_id_str
					});

					writer.pop(); // component_type_name
				});
			}

			void get_resource_query(const Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
			{
				reader.enumerate_array_elements([&scene, &reader, &writer](std::size_t /*i*/)
				{
					// Get the type name
					std::string type_str;
					if (!reader.object_member("type", type_str))
					{
						return;
					}

					// Get the path
					std::string path;
					if (!reader.object_member("path", path) || path.empty())
					{
						return;
					}

					// Get the type from the name
					const auto* type = scene.get_type_db().find_type(type_str.c_str());
					if (!type)
					{
						return;
					}

					// Make sure the type is default-constructible
					if (!type->has_init())
					{
						return;
					}

					// Get the 'from file' implementation
					const auto* from_file = sge::get_vtable<IFromFile>(*type);
					if (!from_file)
					{
						return;
					}

					// Get the 'to archive' implementation
					const auto* to_archive = sge::get_vtable<IToArchive>(*type);
					if (!to_archive)
					{
						return;
					}

					std::cout << "Reading resource '" << path << "'" << std::endl;

					// Construct the type
					auto* object = SGE_STACK_ALLOC(uint8, type->size());
					type->init(object);

					// Load it from the file
					from_file->from_file(object, path.c_str());

					// Save it to the writer
					writer.push_array_element();
					writer.object_member("type", type->name());
					writer.object_member("path", path);
					writer.push_object_member("value");
					to_archive->to_archive(object, writer);
					writer.pop(); // "value"
					writer.pop(); // array element

					// Destroy the resource
					type->drop(object);
				});
			}

			void save_scene_query(const Scene& scene, ArchiveReader& reader)
			{
				std::string path;
				if (!reader.object_member("path", path))
				{
					return;
				}

				// Create an output archive
				JsonArchive output;
				auto* writer = output.write_root();

				// Save the scene
				scene.to_archive(*writer);
				writer->pop();

				// Save the archive to a file
				output.to_file(path.c_str());
				std::cout << "Saved scene to '" << path << "'" << std::endl;
			}

            struct Lightmap
			{
				NodeId node;

				/* Data shared with SceneLightmap::LightmapElement */
                int32 width = 0;
                int32 height = 0;
				byte* direct_mask = nullptr;
				color::RGBF32* basis_x_radiance = nullptr;
				color::RGBF32* basis_y_radiance = nullptr;
				color::RGBF32* basis_z_radiance = nullptr;

				/* Owned data */
                LightmapTexel* lightmap_texels = nullptr;
                byte* lightmap_texel_mask = nullptr;
                color::RGBF32* irradiance_front = nullptr;
				color::RGBF32* irradiance_back = nullptr;
            };

            void generate_lightmaps(Scene& scene, ArchiveReader& reader)
			{
                std::map<std::string, std::unique_ptr<StaticMesh>> meshes;
                std::map<std::string, std::unique_ptr<Material>> materials;
                std::vector<LightmapOccluder> occluders;
                std::vector<LightmapObject> lightmap_objects;
                std::vector<Lightmap> lightmap_object_lightmaps;

				// Load the light from the scene lightmap
            	SceneLightmap scene_lightmap;
				reader.object_member("light_direction", scene_lightmap.light_direction);
				reader.object_member("light_intensity", scene_lightmap.light_intensity);

                // Gather occluders and targets
                std::cout << "Gather scene data..." << std::endl;
                const auto gather_start = std::chrono::high_resolution_clock::now();

				auto* const static_mesh_container = scene.get_component_container(CStaticMesh::type_info);
				NodeId instance_node_ids[8];
				Node* instance_nodes[8];
				CStaticMesh* mesh_instances[8];
				std::size_t start_index = 0;
				std::size_t num_nodes = 0;
				while (static_mesh_container->get_instance_nodes(start_index, 8, &num_nodes, instance_node_ids))
				{
					start_index += 8;
					static_mesh_container->get_instances(instance_node_ids, num_nodes, mesh_instances);
					scene.get_nodes(instance_node_ids, num_nodes, instance_nodes);

					for (std::size_t i = 0; i < num_nodes; ++i)
					{
						const auto node_id = instance_node_ids[i];
						const auto* const node = instance_nodes[i];
						const auto* const mesh = mesh_instances[i];

						if (!mesh->uses_lightmap())
						{
							continue;
						}

						// Try to load the mesh
						StaticMesh* static_mesh;
						{
							const auto mesh_iter = meshes.find(mesh->mesh());
							if (mesh_iter == meshes.end())
							{
								auto new_static_mesh = std::make_unique<StaticMesh>();
								new_static_mesh->from_file(mesh->mesh().c_str());
								static_mesh = new_static_mesh.get();
								meshes.insert(std::make_pair(mesh->mesh(), std::move(new_static_mesh)));
							}
							else
							{
								static_mesh = mesh_iter->second.get();
							}
						}

						// Try to load the material
						Material* material;
						{
							const auto mat_iter = materials.find(mesh->material());
							if (mat_iter == materials.end())
							{
								auto new_material = std::make_unique<Material>();
								new_material->from_file(mesh->material().c_str());
								material = new_material.get();
								materials.insert(std::make_pair(mesh->material(), std::move(new_material)));
							}
							else
							{
								material = mat_iter->second.get();
							}
						}

						// Create the LightmapElement object for this node
						SceneLightmap::LightmapElement lightmap_element;
						lightmap_element.width = mesh->lightmap_width();
						lightmap_element.height = mesh->lightmap_height();
						lightmap_element.basis_x_radiance.assign(lightmap_element.width * lightmap_element.height, color::RGBF32::black());
						lightmap_element.basis_y_radiance.assign(lightmap_element.width * lightmap_element.height, color::RGBF32::black());
						lightmap_element.basis_z_radiance.assign(lightmap_element.width * lightmap_element.height, color::RGBF32::black());
						lightmap_element.direct_mask.assign(lightmap_element.width * lightmap_element.height, 0);

						// Add the object to the list of occluders
						LightmapOccluder occluder;
						occluder.mesh = static_mesh;
						occluder.world_transform = node->get_world_matrix();
						occluders.push_back(occluder);

						// Add the object to the list of lightmap objects
						LightmapObject object;
						object.mesh = static_mesh;
						object.world_transform = occluder.world_transform;
						object.base_color = material->base_reflectivity();
						lightmap_objects.push_back(object);

						// Create a lightmap object for this object
						Lightmap lightmap;
						lightmap.node = node_id;
						lightmap.width = lightmap_element.width;
						lightmap.height = lightmap_element.height;
						lightmap.direct_mask = lightmap_element.direct_mask.data();
						lightmap.basis_x_radiance = lightmap_element.basis_x_radiance.data();
						lightmap.basis_y_radiance = lightmap_element.basis_y_radiance.data();
						lightmap.basis_z_radiance = lightmap_element.basis_z_radiance.data();
						lightmap.lightmap_texels = (LightmapTexel*)std::calloc(lightmap.width * lightmap.height, sizeof(LightmapTexel));
						lightmap.lightmap_texel_mask = (byte*)std::calloc(lightmap.width * lightmap.height, 1);
						lightmap.irradiance_front = (color::RGBF32*)std::calloc(lightmap.width * lightmap.height, sizeof(color::RGBF32));
						lightmap.irradiance_back = (color::RGBF32*)std::calloc(lightmap.width * lightmap.height, sizeof(color::RGBF32));

						// Add the lightmap to the list of lightmaps
						lightmap_object_lightmaps.push_back(std::move(lightmap));

						// Add the lightmap element to the scene lightmap datastructure
						scene_lightmap.lightmap_elements.insert(std::make_pair(node_id, std::move(lightmap_element)));
					}
				}
                const auto gather_end = std::chrono::high_resolution_clock::now();

                // Debug time
                std::cout << "Gathered scene data in ";
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(gather_end - gather_start).count();
                std::cout << " milliseconds." << std::endl;

                // Create the lightmap scene
                std::cout << "Constructing lightmap scene structure..." << std::endl;
                const auto scene_construct_start = std::chrono::high_resolution_clock::now();
                auto* lm_scene = new_lightmap_scene(occluders.data(), occluders.size());
                const auto scene_construct_end = std::chrono::high_resolution_clock::now();

                // Debug time
                std::cout << "Constructed lightmap scene structure in ";
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(scene_construct_end - scene_construct_start).count();
                std::cout << " milliseconds." << std::endl;

                // Generate lightmap texels for all objects
                std::cout << "Generating lightmap texel information..." << std::endl;
                const auto gen_tex_start = std::chrono::high_resolution_clock::now();
                for (std::size_t i = 0; i < lightmap_objects.size(); ++i)
                {
                    auto& object = lightmap_objects[i];
                    auto& lightmap = lightmap_object_lightmaps[i];
                    sge::generate_lightmap_texels(
                        &object,
                        1,
                        lightmap.width,
                        lightmap.height,
                        lightmap.lightmap_texels,
                        lightmap.lightmap_texel_mask);
                }
                const auto gen_tex_end = std::chrono::high_resolution_clock::now();

                // Debug time
                std::cout << "Generated lightmap texel information in ";
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(gen_tex_end - gen_tex_start).count();
                std::cout << " milliseconds." << std::endl;

                // Create a light
                sge::LightmapLight light;
				light.direction = scene_lightmap.light_direction;
					light.intensity = scene_lightmap.light_intensity;

                // Compute direct lighting for all objects
                std::cout << "Computing direct lighting..." << std::endl;
                const auto gen_direct_start = std::chrono::high_resolution_clock::now();
                for (std::size_t i = 0; i < lightmap_objects.size(); ++i)
                {
                    auto& occluder = occluders[i];
                    auto& lightmap = lightmap_object_lightmaps[i];
                    sge::compute_direct_irradiance(
                        lm_scene,
                        light,
                        lightmap.width,
                        lightmap.height,
                        lightmap.lightmap_texels,
                        lightmap.lightmap_texel_mask,
                        lightmap.irradiance_back);

                    auto* const irradiance_front = lightmap.irradiance_front;
					auto* const irradiance_back = lightmap.irradiance_back;
					auto* const direct_mask = lightmap.direct_mask;
                	const auto lightmap_size = lightmap.width * lightmap.height;
					for (int32 pix_i = 0; pix_i < lightmap_size; ++pix_i)
					{
						const auto irradiance_value = irradiance_back[pix_i];

						// Copy irradiance to front buffer
						irradiance_front[pix_i] = irradiance_value;

						// Compute mask value
						const auto mask = irradiance_value.red() != 0.f || irradiance_value.green() != 0.f || irradiance_value.blue() != 0.f;
						direct_mask[pix_i] = mask ? 0xFF : 0x0;
					}

                    // Set irradiance on the occluder
                    occluder.irradiance_width = lightmap.width;
                    occluder.irradiance_height = lightmap.height;
                    occluder.irradiance = irradiance_front;
                }
                const auto gen_direct_end = std::chrono::high_resolution_clock::now();

                // Debug time
                std::cout << "Computed direct lighting in ";
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(gen_direct_end - gen_direct_start).count();
                std::cout << " milliseconds.\n";

                // Compute indirect lighting for all objects
				int32 num_sample_sets = 16;
				int32 num_accumulation_steps = 1;
				reader.object_member("num_indirect_sample_sets", num_sample_sets);
				reader.object_member("num_accumulation_steps", num_accumulation_steps);
                std::cout << "Computing indirect lighting (" << num_sample_sets << " sample sets, " << num_accumulation_steps << " accumulation steps)..." << std::endl;
                const auto gen_indirect_start = std::chrono::high_resolution_clock::now();
                const int num_threads = std::thread::hardware_concurrency();
                for (int32 pass_i = 0; pass_i < num_accumulation_steps; ++pass_i)
                {
                    for (std::size_t i = 0; i < lightmap_objects.size(); ++i)
                    {
                        auto& lightmap = lightmap_object_lightmaps[i];

                        // Create jobs
                        const auto job_split = lightmap.height / num_threads;
                        std::vector<std::future<void>> jobs;
                        for (int job_i = 0; job_i < num_threads - 1; ++job_i)
                        {
							const auto job_lightmap_offset = job_i * job_split * lightmap.width;
                            jobs.push_back(std::async(std::launch::async, sge::compute_indirect_radiance,
                                lm_scene,
                                num_sample_sets,
								num_accumulation_steps,
                                lightmap.width,
                                job_split,
                                lightmap.lightmap_texels + job_lightmap_offset,
                                lightmap.lightmap_texel_mask + job_lightmap_offset,
                                lightmap.basis_x_radiance + job_lightmap_offset,
								lightmap.basis_y_radiance + job_lightmap_offset,
								lightmap.basis_z_radiance + job_lightmap_offset,
								lightmap.irradiance_back + job_lightmap_offset));
                        }

                        // Compute final slice
						const auto final_lightmap_offset = (num_threads - 1) * job_split * lightmap.width;
                        sge::compute_indirect_radiance(
                            lm_scene,
                            num_sample_sets,
							num_accumulation_steps,
                            lightmap.width,
                            lightmap.height - (num_threads - 1) * job_split,
                            lightmap.lightmap_texels + final_lightmap_offset,
                            lightmap.lightmap_texel_mask + final_lightmap_offset,
                            lightmap.basis_x_radiance + final_lightmap_offset,
							lightmap.basis_y_radiance + final_lightmap_offset,
							lightmap.basis_z_radiance + final_lightmap_offset,
							lightmap.irradiance_back + final_lightmap_offset);
                    }

                    // Copy irradiance back buffer to front buffer
                    for (std::size_t i = 0; i < lightmap_objects.size(); ++i)
                    {
                        auto& lightmap = lightmap_object_lightmaps[i];
                        std::memcpy(
                            lightmap.irradiance_front,
                            lightmap.irradiance_back,
                            lightmap.width * lightmap.height * sizeof(color::RGBF32));
                    }
                }
                const auto gen_indirect_end = std::chrono::high_resolution_clock::now();

                // Debug time
                std::cout << "Computed indirect lighting in ";
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(gen_indirect_end - gen_indirect_start).count();
                std::cout << " milliseconds." << std::endl;

				// Compute ambient lighting for all objects
				auto ambient = color::RGBF32::black();
				reader.object_member("ambient", ambient);
				std::cout << "Computing ambient lighting...\n";
				const auto gen_ambient_start = std::chrono::high_resolution_clock::now();
				for (size_t i = 0; i < lightmap_objects.size(); ++i)
				{
					auto& lightmap = lightmap_object_lightmaps[i];
					compute_ambient_radiance(
						ambient,
						lightmap.width,
						lightmap.height,
						lightmap.lightmap_texel_mask,
						lightmap.basis_x_radiance);
					compute_ambient_radiance(
						ambient,
						lightmap.width,
						lightmap.height,
						lightmap.lightmap_texel_mask,
						lightmap.basis_y_radiance);
					compute_ambient_radiance(
						ambient,
						lightmap.width,
						lightmap.height,
						lightmap.lightmap_texel_mask,
						lightmap.basis_z_radiance);
				}
				const auto gen_ambient_end = std::chrono::high_resolution_clock::now();

				// Debug time
				std::cout << "Computed ambient lighting in ";
				std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(gen_ambient_end - gen_ambient_start).count();
				std::cout << " milliseconds.\n";

                // Post-process lightmaps
				int32 num_post_steps = 4;
				reader.object_member("post_process_steps", num_post_steps);
                std::cout << "Post-processing..." << std::endl;
            	const auto post_start = std::chrono::high_resolution_clock::now();

                for (auto& lightmap : lightmap_object_lightmaps)
                {
					// Run post-processing
                    sge::lightmap_postprocess(lightmap.width, lightmap.height, num_post_steps, lightmap.basis_x_radiance);
					sge::lightmap_postprocess(lightmap.width, lightmap.height, num_post_steps, lightmap.basis_y_radiance);
					sge::lightmap_postprocess(lightmap.width, lightmap.height, num_post_steps, lightmap.basis_z_radiance);

                    // Free owned data
                    std::free(lightmap.lightmap_texels);
                    std::free(lightmap.lightmap_texel_mask);
					std::free(lightmap.irradiance_front);
					std::free(lightmap.irradiance_back);
                }
                const auto post_end = std::chrono::high_resolution_clock::now();

                // Debug time
                std::cout << "Post processed in ";
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(post_end - post_start).count();
                std::cout << " milliseconds." << std::endl;

                // Clean up scene
                free_lightmap_scene(lm_scene);

				// Save lightmap to archive
				BinaryArchive lightmap_out;
				auto* lightmap_writer = lightmap_out.write_root();
				scene_lightmap.to_archive(*lightmap_writer);
				lightmap_writer->pop();

				// Save archive to file
            	std::string lightmap_path;
				reader.object_member("lightmap_path", lightmap_path);
				lightmap_out.to_file(lightmap_path.c_str());

				// Assign lightmap to scene
				scene.get_raw_scene_data().lightmap_data_path = std::move(lightmap_path);
			}
		}
	}
}
