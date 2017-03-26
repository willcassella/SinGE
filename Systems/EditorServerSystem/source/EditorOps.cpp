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

			static void read_properties(Any<> object, ArchiveWriter* const* writers, std::size_t num_writers)
			{
				object.type().enumerate_properties([object, writers, num_writers](const char* propName, const PropertyInfo& propInfo)
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

					for (std::size_t i = 0; i < num_writers; ++i)
					{
						writers[i]->push_object_member(propName);
					}

					// Access the property
					propInfo.get(object.object(), [writers, num_writers](Any<> prop)
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

							for (std::size_t i = 0; i < num_writers; ++i)
							{
								impl->to_archive(prop.object(), *writers[i]);
							}

							return;
						}

						read_properties(prop, writers, num_writers);
					});

					for (std::size_t i = 0; i < num_writers; ++i)
					{
						writers[i]->pop();
					}
				});
			}

			static void write_properties(AnyMut<> object, ArchiveReader& reader)
			{
				reader.enumerate_object_members([object, &reader](const char* propName)
				{
					// Search for the property
					const auto* propInfo = object.type().find_property(propName);
					if (!propInfo)
					{
						return;
					}

					// Modify the property
					propInfo->mutate(object.object(), [&reader](AnyMut<> prop)
					{
						// If we need to recurse deeper
						if (reader.is_object())
						{
							write_properties(prop, reader);
						}

						// Get the FromArchive impl for this type
						const auto* const impl = sge::get_vtable<IFromArchive>(prop.type());
						if (!impl)
						{
							return;
						}

						impl->from_archive(prop.object(), reader);
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

					// If the type was found
					if (type)
					{
						std::cout << "Sending type info for " << typeName << std::endl;
						writer.push_object_member(typeName.c_str());
						read_type_info(*type, writer);
						writer.pop();
					}
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

			void new_node_query(Scene& scene, ArchiveReader& reader)
			{
				// Get the number of nodes to create
				uint32 num_nodes;
				if (!reader.number(num_nodes))
				{
					return;
				}

				// Create the nodes
				for (uint32 i = 0; i < num_nodes; ++i)
				{
                    Node* node = nullptr;
					scene.create_nodes(1, &node);
					std::cout << "Created node " << node->get_id().to_u64() << std::endl;
				}
			}

			void destroy_node_query(Scene& scene, ArchiveReader& reader)
			{
				reader.enumerate_array_elements([&scene, &reader](std::size_t /*i*/)
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
				});
			}

			void set_node_name_query(Scene& scene, ArchiveReader& reader)
			{
				reader.enumerate_object_members([&scene, &reader](const char* node_id_str)
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
				});
			}

			void set_node_root_query(Scene& scene, ArchiveReader& reader)
			{
				reader.enumerate_object_members([&scene, &reader](const char* node_id_str)
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
				});
			}

			void new_component_query(Scene& scene, ArchiveReader& reader)
			{
				// Enumerate nodes to add components to
				reader.enumerate_object_members([&scene, &reader](const char* node_id_str)
				{
					NodeId node_id;
					node_id.from_string(node_id_str);

					// Enumerate component types to add
					reader.enumerate_array_elements([node_id, &scene, &reader](std::size_t /*i*/)
					{
						std::string component_type;
						sge::from_archive(component_type, reader);

						// Get the component type
						const auto* const type = scene.get_component_type(component_type.c_str());
						if (!type)
						{
							std::cout << "Error: Invalid component type name: '" << component_type << "'" << std::endl;
							return;
						}

						// Get the component container
						auto* const container = scene.get_component_container(*type);
						if (!container)
						{
							std::cout << "Error: Invalid component type: '" << component_type << "'" << std::endl;
							return;
						}

						// Create the component
						void* instance = nullptr;
						container->create_instances(&node_id, 1, &instance);
						std::cout << "Created '" << type->name() << "' component on node " << node_id.to_u64() << std::endl;
					});
				});
			}

			void destroy_component_query(Scene& scene, ArchiveReader& reader)
			{
				// Enumerate component types to destroy
				reader.enumerate_object_members([&scene, &reader](const char* component_type_name)
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

					// Enumerate instances to destroy
					reader.enumerate_array_elements([component_type_name, container, &scene, &reader](std::size_t /*i*/)
					{
						// Get the node
						NodeId node;
						node.from_archive(reader);

						// Destroy it
						container->remove_instances(&node, 1);
						std::cout << "Destroyed '" << component_type_name << "' component on node " << node.to_u64() << std::endl;
					});
				});
			}

			void get_scene_query(Scene& scene, ArchiveWriter& writer)
			{
				const auto& scene_data = scene.get_raw_scene_data();
				std::cout << "Sending scene information" << std::endl;

				// Iterate over nodes
				writer.push_object_member("nodes");
				for (auto node : scene_data.nodes)
				{
					char node_id_str[20];
					node.first.to_string(node_id_str, 20);

					writer.push_object_member(node_id_str);
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
				for (const auto& component_type : scene_data.components)
				{
					NodeId instance_nodes[8];
					std::size_t num_nodes = 0;
					std::size_t start_index = 0;

					writer.push_object_member(component_type.first->name().c_str());

					// Get instances of this component
					while (component_type.second->get_instance_nodes(start_index, 8, &num_nodes, instance_nodes))
					{
						start_index += 8;
						for (std::size_t i = 0; i < num_nodes; ++i)
						{
							writer.array_element(instance_nodes[i]);
						}
					}

				    writer.pop(); // type name
				}
				writer.pop(); // "components"
			}

			void get_component_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
			{
				// Enumerate the types to get the properties from
				reader.enumerate_object_members([&scene, &reader, &writer](const char* component_type_name)
				{
					// Get the type
					const auto* const type = scene.get_component_type(component_type_name);
					if (!type)
					{
						std::cout << "Error: Invalid component type name: '" << component_type_name << "'" << std::endl;
						return;
					}

					// Get the container
					auto* const container = scene.get_component_container(*type);
					if (!container)
					{
						std::cout << "Error: Invalid component type: '" << component_type_name << "'" << std::endl;
						return;
					}

					writer.push_object_member(component_type_name);

					// Enumerate the instances
					reader.enumerate_array_elements([container, type, &scene, &reader, &writer](std::size_t /*i*/)
					{
						// Get the node id
						NodeId node_id;
						node_id.from_archive(reader);

						// Access the component
						void* component;
						container->get_instances(&node_id, 1, &component);
						if (!component)
						{
							std::cout << "Invalid node id: " << node_id.to_u64() << std::endl;
							return;
						}

						// Output the Id
						char node_id_str[20];
						node_id.to_string(node_id_str, 20);
						writer.push_object_member(node_id_str);

						// Access the component
						std::cout << "Reading properties of '" << type->name() << "' component on node " << node_id_str << std::endl;
						ArchiveWriter* writers[] = { &writer };
						read_properties(Any<>{ *type, component }, writers, 1);

						writer.pop(); // node_id_str
					});

					writer.pop(); // component_type_name
				});
			}

			void set_component_query(Scene& scene, ArchiveReader& reader)
			{
				// Enumerate types of components changed
				reader.enumerate_object_members([&scene, &reader](const char* component_type_name)
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

					// Enumerate the node Ids of changed components
					reader.enumerate_object_members([type, container, &scene, &reader](const char* node_id_str)
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
						write_properties(AnyMut<>{ *type, component }, reader);
					});
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

            void generate_lightmaps(Scene& scene)
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

					for (std::size_t i = 0; i < 8; ++i)
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
						occluder.base_color = material->base_color();
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
                std::cout << "Computing indirect lighting..." << std::endl;
                const auto gen_indirect_start = std::chrono::high_resolution_clock::now();
                const int num_threads = std::thread::hardware_concurrency();
                for (std::size_t pass_i = 0; pass_i < 5; ++pass_i)
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
                                16,
                                lightmap.width,
                                job_split,
                                lightmap.lightmap_texels + job_i * job_split * lightmap.width,
                                lightmap.lightmap_texel_mask + job_i * job_split * lightmap.width,
                                lightmap.lightmap_pixels + job_i * job_split * lightmap.width));
                        }

                        // Compute final slice
                        sge::compute_indirect_irradiance(
                            lm_scene,
                            16,
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
                    sge::postprocess_irradiance(lightmap.width, lightmap.height, 4, lightmap.lightmap_pixels);
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
