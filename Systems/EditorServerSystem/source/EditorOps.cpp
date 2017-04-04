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
#include <Core/Interfaces/IFromString.h>
#include <Resource/Interfaces/IFromFile.h>
#include <Resource/Archives/JsonArchive.h>
#include <Resource/Resources/StaticMesh.h>
#include <Resource/Resources/Image.h>
#include <Lightmapper/Lightmapper.h>
#include <Engine/Scene.h>
#include <Engine/SystemFrame.h>
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
                std::string path;
                int32 width = 0;
                int32 height = 0;
                LightmapTexel* lightmap_texels = nullptr;
                byte* lightmap_texel_mask = nullptr;
                color::RGBAF32* lightmap_pixels = nullptr;
                color::RGBAF32* irradiance_pixels = nullptr;
            };

            void generate_lightmaps(Scene& scene, ArchiveReader& reader)
			{
                std::map<std::string, std::unique_ptr<StaticMesh>> meshes;
                std::map<std::string, std::unique_ptr<Material>> materials;
                std::vector<LightmapOccluder> occluders;
                std::vector<LightmapObject> lightmap_objects;
                std::vector<Lightmap> lightmap_object_lightmaps;

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
						auto* const node = instance_nodes[i];
						auto* const mesh = mesh_instances[i];

						if (!mesh->uses_lightmap())
						{
							continue;
						}

						StaticMesh* static_mesh = nullptr;

						// Try to load the mesh
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

						Material* material = nullptr;

						// Try to load the material
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

						// Add the object to the list of occluders
						LightmapOccluder occluder;
						occluder.mesh = static_mesh;
						occluder.base_color = material->base_reflectivity();
						occluder.world_transform = node->get_world_matrix();
						occluders.push_back(occluder);

						// Add the object to the list of lightmap objects
						LightmapObject object;
						object.mesh = static_mesh;
						object.world_transform = occluder.world_transform;
						lightmap_objects.push_back(object);

						// Get the path for the lightmap
						auto lightmap_path = mesh->lightmap();
						if (lightmap_path.empty())
						{
							lightmap_path = "Content/Lightmaps/" + node->get_name() + ".exr";
							mesh->lightmap(lightmap_path);
						}

						// Create a lightmap object for this object
						Lightmap lightmap;
						lightmap.path = std::move(lightmap_path);
						lightmap.width = mesh->lightmap_width();
						lightmap.height = mesh->lightmap_height();
						lightmap.lightmap_texels = (LightmapTexel*)std::calloc(lightmap.width * lightmap.height, sizeof(LightmapTexel));
						lightmap.lightmap_texel_mask = (byte*)std::calloc(lightmap.width * lightmap.height, 1);
						lightmap.irradiance_pixels = (color::RGBAF32*)std::calloc(lightmap.width * lightmap.height, sizeof(color::RGBAF32));
						lightmap.lightmap_pixels = (color::RGBAF32*)std::calloc(lightmap.width * lightmap.height, sizeof(color::RGBAF32));

						// Add the lightmap to the list of lightmaps
						lightmap_object_lightmaps.push_back(std::move(lightmap));
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
                light.direction = sge::Vec3{ 0, -0.5f, -0.5f }.normalized();
                light.intensity = sge::color::RGBF32{ 0.5f, 0.5f, 0.5f };
				reader.object_member("light_direction", light.direction);
				reader.object_member("light_intensity", light.intensity);

                // Compute direct lighting for all objects
                std::cout << "Computing direction lighting..." << std::endl;
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
                        lightmap.irradiance_pixels);

                    // Copy alpha component to lightmap
                    auto* const lightmap_pixels = lightmap.lightmap_pixels;
                    const auto* const irradiance_pixels = lightmap.irradiance_pixels;
                    const auto lightmap_size = lightmap.width * lightmap.height;
                    for (std::size_t pix_i = 0; pix_i < lightmap_size; ++pix_i)
                    {
                        const float alpha = irradiance_pixels[pix_i].alpha();
                        lightmap_pixels[pix_i].alpha(std::ceil(alpha));
                    }

                    // Set pixel data to the occluder
                    occluder.irradiance_width = lightmap.width;
                    occluder.irradiance_height = lightmap.height;
                    occluder.irradiance = irradiance_pixels;
                }
                const auto gen_direct_end = std::chrono::high_resolution_clock::now();

                // Debug time
                std::cout << "Computed direct lighting in ";
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(gen_direct_end - gen_direct_start).count();
                std::cout << " milliseconds." << std::endl;

                // Compute indirect lighting for all objects
				int32 num_sample_sets = 16;
				reader.object_member("num_indirect_sample_sets", num_sample_sets);
                std::cout << "Computing indirect lighting..." << std::endl;
                const auto gen_indirect_start = std::chrono::high_resolution_clock::now();
                const int num_threads = std::thread::hardware_concurrency();
                for (std::size_t pass_i = 0; pass_i < 1; ++pass_i)
                {
                    for (std::size_t i = 0; i < lightmap_objects.size(); ++i)
                    {
                        auto& lightmap = lightmap_object_lightmaps[i];

                        // Create jobs
                        const auto job_split = lightmap.height / num_threads;
                        std::vector<std::future<void>> jobs;
                        for (int job_i = 0; job_i < num_threads - 1; ++job_i)
                        {
                            jobs.push_back(std::async(std::launch::async, sge::compute_indirect_irradiance,
                                lm_scene,
                                num_sample_sets,
                                lightmap.width,
                                job_split,
                                lightmap.lightmap_texels + job_i * job_split * lightmap.width,
                                lightmap.lightmap_texel_mask + job_i * job_split * lightmap.width,
                                lightmap.lightmap_pixels + job_i * job_split * lightmap.width));
                        }

                        // Compute final slice
                        sge::compute_indirect_irradiance(
                            lm_scene,
                            num_sample_sets,
                            lightmap.width,
                            lightmap.height - (num_threads - 1) * job_split,
                            lightmap.lightmap_texels + (num_threads - 1) * job_split * lightmap.width,
                            lightmap.lightmap_texel_mask + (num_threads - 1) * job_split * lightmap.width,
                            lightmap.lightmap_pixels + (num_threads - 1) * job_split * lightmap.width);
                        }

                    // Copy to irradiance
                    for (std::size_t i = 0; i < lightmap_objects.size(); ++i)
                    {
                        auto& lightmap = lightmap_object_lightmaps[i];
                        std::memcpy(
                            lightmap.irradiance_pixels,
                            lightmap.lightmap_pixels,
                            lightmap.width * lightmap.height * sizeof(color::RGBAF32));
                    }
                }
                const auto gen_indirect_end = std::chrono::high_resolution_clock::now();

                // Debug time
                std::cout << "Computed indirect lighting in ";
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(gen_indirect_end - gen_indirect_start).count();
                std::cout << " milliseconds." << std::endl;

                // Post-process and save all lightmap
                std::cout << "Post-processing..." << std::endl;
                const auto post_start = std::chrono::high_resolution_clock::now();
                for (auto& lightmap : lightmap_object_lightmaps)
                {
					int32 num_post_steps = 4;
					reader.object_member("post_process_steps", num_post_steps);
                    sge::postprocess_irradiance(lightmap.width, lightmap.height, num_post_steps, lightmap.lightmap_pixels);
                    Image::save_rgbf(lightmap.lightmap_pixels->vec(), lightmap.width, lightmap.height, 4, lightmap.path.c_str());

                    // Free data
                    std::free(lightmap.lightmap_pixels);
                    std::free(lightmap.irradiance_pixels);
                    std::free(lightmap.lightmap_texels);
                    std::free(lightmap.lightmap_texel_mask);
                }
                const auto post_end = std::chrono::high_resolution_clock::now();

                // Debug time
                std::cout << "Post processed in ";
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(post_end - post_start).count();
                std::cout << " milliseconds." << std::endl;

                // Clean up scene
                free_lightmap_scene(lm_scene);
			}
		}
	}
}
