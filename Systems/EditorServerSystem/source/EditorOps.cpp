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
#include <Engine/Components/CTransform3D.h>
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

			void new_entity_query(SystemFrame& frame, ArchiveReader& reader)
			{
				// Get the number of entities to create
				uint32 num_entities;
				if (!reader.number(num_entities))
				{
					return;
				}

				// Create the entities
				for (uint32 i = 0; i < num_entities; ++i)
				{
                    EntityId entity = NULL_ENTITY;
					frame.create_entities(&entity, 1);
					std::cout << "Created entity " << entity << std::endl;
				}
			}

			void destroy_entity_query(SystemFrame& frame, ArchiveReader& reader)
			{
				// Enumerate entities to destroy
				reader.enumerate_array_elements([&frame, &reader](std::size_t /*i*/)
				{
					// Get the entity id
					EntityId entity = NULL_ENTITY;
					sge::from_archive(entity, reader);

					// Destroy it
					std::cout << "Destroying entity " << entity << std::endl;
                    frame.destroy_entities(&entity, 1);
				});
			}

			void set_entity_name_query(SystemFrame& frame, ArchiveReader& reader)
			{
				// Enumerate entities to set the names of
				reader.enumerate_object_members([&frame, &reader](const char* entity_id_str)
				{
					// Get the entity to set the name of
					EntityId entity_id = NULL_ENTITY;
					sge::from_string(entity_id, entity_id_str, std::strlen(entity_id_str));

					// Get the name to set
					std::string name;
					sge::from_archive(name, reader);

					// Set the name
					std::cout << "Setting name on entity " << entity_id << " to '" << name << "'" << std::endl;
					frame.set_entity_name(entity_id, std::move(name));
				});
			}

			void set_entity_parent_query(SystemFrame& frame, ArchiveReader& reader)
			{
				// Enumerate entities to set the names of
				reader.enumerate_object_members([&frame, &reader](const char* entity_id_str)
				{
					// Get the id of the entity to set the parent of
					EntityId entity_id = NULL_ENTITY;
					sge::from_string(entity_id, entity_id_str, std::strlen(entity_id_str));

					// Get the id of the entity to set as the parent
					EntityId parent_id = NULL_ENTITY;
					sge::from_archive(parent_id, reader);

					// Set the parent
					frame.set_entities_parent(parent_id, &entity_id, 1);
					std::cout << "Setting parent on entity " << entity_id << " to " << parent_id << std::endl;
				});
			}

			void new_component_query(SystemFrame& frame, ArchiveReader& reader)
			{
				// Enumerate entities to add components to
				reader.enumerate_object_members([&frame, &reader](const char* entity_id_str)
				{
					EntityId entity_id;
					sge::from_string(entity_id, entity_id_str, std::strlen(entity_id_str));

					// Enumerate component types to add
					reader.enumerate_array_elements([entity_id, &frame, &reader](std::size_t /*i*/)
					{
						std::string component_type;
						sge::from_archive(component_type, reader);

						// Get the component type
						const auto* type = frame.scene().get_component_type(component_type.c_str());
						if (!type)
						{
							return;
						}

						// Create the component
						frame.create_components(*type, &entity_id, 1);
						std::cout << "Created '" << type->name() << "' component on entity " << entity_id << std::endl;
					});
				});
			}

			void destroy_component_query(SystemFrame& frame, ArchiveReader& reader)
			{
				// Enumerate componen types to destroy
				reader.enumerate_object_members([&frame, &reader](const char* component_type_name)
				{
					// Get the component type
					const auto* type = frame.scene().get_component_type(component_type_name);
					if (!type)
					{
						return;
					}

					// Enumerate instances to destroy
					reader.enumerate_array_elements([type, &frame, &reader](std::size_t /*i*/)
					{
						// Get the entity
						EntityId entity = NULL_ENTITY;
						sge::from_archive(entity, reader);

						// Destroy it
						std::cout << "Destroyed '" << type->name() << "' component on entity " << entity << std::endl;
						frame.destroy_components(*type, &entity, 1);
					});
				});
			}

			void get_scene_query(const SystemFrame& frame, ArchiveWriter& writer)
			{
				const auto& scene_data = frame.scene().get_raw_scene_data();
				std::cout << "Sending scene information" << std::endl;
				writer.object_member("next_entity_id", scene_data.next_entity_id);

				// Iterate over entities
				writer.push_object_member("entities");
				for (auto entity : scene_data.entity_parents)
				{
					writer.push_object_member(sge::to_string(entity.first).c_str());
					writer.object_member("name", frame.get_entity_name(entity.first));
					writer.object_member("parent", frame.get_entity_parent(entity.first));
					writer.pop(); // entity
				}
				writer.pop(); // "entities"

				// Iterate over components
				writer.push_object_member("components");
				for (const auto& component_type : scene_data.components)
				{
                    const auto begin = component_type.second->get_instance_range();
                    const std::size_t len = component_type.second->get_instance_range_length();

					// Skip this component if there are no entities
					if (len == 0)
					{
						continue;
					}

					// Write instances
					writer.push_object_member(component_type.first->name().c_str());
                    writer.typed_array(begin, len);
				    writer.pop(); // type name
				}
				writer.pop(); // "components"
			}

			void get_component_query(SystemFrame& frame, ArchiveReader& reader, ArchiveWriter& writer)
			{
				// Enumerate the types to get the properties from
				reader.enumerate_object_members([&frame, &reader, &writer](const char* typeName)
				{
					writer.push_object_member(typeName);

					// Get the type
					const auto* type = frame.scene().get_component_type(typeName);
					if (!type)
					{
						return;
					}

					// Enumerate the instances
					reader.enumerate_array_elements([type, &frame, &reader, &writer](std::size_t /*i*/)
					{
						// Get the current entity id
						EntityId entity_id;
						sge::from_archive(entity_id, reader);
						writer.push_object_member(sge::to_string(entity_id).c_str());

						// Access the component
                        const EntityId* proc_range[] = { &entity_id };
                        const std::size_t range_len = 1;
						frame.process_entities(proc_range, &range_len, 1, 0, 1, &type, 1,
                            [type, entity_id, &writer](ProcessingFrame&, auto comp) -> ProcessControl
						{
							std::cout << "Reading properties of '" << type->name() << "' component on entity '" << entity_id << "'" << std::endl;
							ArchiveWriter* writers[] = { &writer };
							read_properties(Any<>{ *type, comp[0] }, writers, 1);
							return ProcessControl::BREAK;
						});

						writer.pop();
					});

					writer.pop();
				});
			}

			void set_component_query(SystemFrame& frame, ArchiveReader& reader)
			{
				// Enumerate types of components changed
				reader.enumerate_object_members([&frame, &reader](const char* typeName)
				{
					auto* type = frame.scene().get_component_type(typeName);
					if (!type)
					{
						return;
					}

					// Enumerate the EntityIds of components changed
					reader.enumerate_object_members([type, &frame, &reader](const char* entityId)
					{
                        const EntityId entity_id = std::strtoull(entityId, nullptr, 10);

					    // Process the component and deserialize it
                        const EntityId* proc_range[] = { &entity_id };
                        const std::size_t range_len = 1;
						frame.process_entities_mut(proc_range, &range_len, 1, 0, 1, &type, 1,
                            [type, &reader](ProcessingFrame& pframe, auto comp) -> ProcessControl
						{
							std::cout << "Writing properties of '" << type->name() << "' component on entity '" << pframe.entity() << "'" << std::endl;
							write_properties(AnyMut<>{ *type, comp[0] }, reader);
							return ProcessControl::BREAK;
						});
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

            void generate_lightmaps(SystemFrame& frame)
			{
                std::map<std::string, std::unique_ptr<StaticMesh>> meshes;
                std::map<std::string, std::unique_ptr<Material>> materials;
                std::vector<LightmapOccluder> occluders;
                std::vector<LightmapObject> lightmap_objects;
                std::vector<Lightmap> lightmap_object_lightmaps;

                // Gather occluders and targets
                std::cout << "Gather scene data..." << std::endl;
                const auto gather_start = std::chrono::high_resolution_clock::now();
			    frame.process_entities_mut([&](
                    ProcessingFrame& pframe,
                    CTransform3D& transform,
                    CStaticMesh& mesh)
			    {
                    if (!mesh.uses_lightmap())
                    {
                        return ProcessControl::CONTINUE;
                    }

                    StaticMesh* static_mesh = nullptr;

                    // Try to load the mesh
                    {
                        const auto mesh_iter = meshes.find(mesh.mesh());
                        if (mesh_iter == meshes.end())
                        {
                            auto new_static_mesh = std::make_unique<StaticMesh>();
                            new_static_mesh->from_file(mesh.mesh().c_str());
                            static_mesh = new_static_mesh.get();
                            meshes.insert(std::make_pair(mesh.mesh(), std::move(new_static_mesh)));
                        }
                        else
                        {
                            static_mesh = mesh_iter->second.get();
                        }
                    }

                    Material* material = nullptr;

                    // Try to load the material
                    {
                        const auto mat_iter = materials.find(mesh.material());
                        if (mat_iter == materials.end())
                        {
                            auto new_material = std::make_unique<Material>();
                            new_material->from_file(mesh.material().c_str());
                            material = new_material.get();
                            materials.insert(std::make_pair(mesh.material(), std::move(new_material)));
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
                    occluder.world_transform = transform.get_world_matrix();
                    occluders.push_back(occluder);

                    // Add the object to the list of lightmap objects
                    LightmapObject object;
                    object.mesh = static_mesh;
                    object.world_transform = occluder.world_transform;
                    lightmap_objects.push_back(object);

                    // Get the path for the lightmap
                    auto lightmap_path = mesh.lightmap();
                    if (lightmap_path.empty())
                    {
                        lightmap_path = "Content/Lightmaps/" + frame.get_entity_name(pframe.entity()) + ".exr";
                        mesh.lightmap(lightmap_path);
                    }

                    // Create a lightmap object for this object
                    Lightmap lightmap;
                    lightmap.path = std::move(lightmap_path);
                    lightmap.width = mesh.lightmap_width();
                    lightmap.height = mesh.lightmap_height();
                    lightmap.lightmap_texels = (LightmapTexel*)std::calloc(lightmap.width * lightmap.height, sizeof(LightmapTexel));
                    lightmap.lightmap_texel_mask = (byte*)std::calloc(lightmap.width * lightmap.height, 1);
                    lightmap.irradiance_pixels = (color::RGBAF32*)std::calloc(lightmap.width * lightmap.height, sizeof(color::RGBAF32));
                    lightmap.lightmap_pixels = (color::RGBAF32*)std::calloc(lightmap.width * lightmap.height, sizeof(color::RGBAF32));

                    // Add the lightmap to the list of lightmaps
			        lightmap_object_lightmaps.push_back(std::move(lightmap));
                    return ProcessControl::CONTINUE;
                });
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
                            32,
                            lightmap.width,
                            job_split,
                            lightmap.lightmap_texels + job_i * job_split * lightmap.width,
                            lightmap.lightmap_texel_mask + job_i * job_split * lightmap.width,
                            lightmap.lightmap_pixels + job_i * job_split * lightmap.width));
                    }

                    // Compute final slice
                    sge::compute_indirect_irradiance(
                        lm_scene,
                        32,
                        lightmap.width,
                        lightmap.height - (num_threads - 1) * job_split,
                        lightmap.lightmap_texels + (num_threads - 1) * job_split * lightmap.width,
                        lightmap.lightmap_texel_mask + (num_threads - 1) * job_split * lightmap.width,
                        lightmap.lightmap_pixels + (num_threads - 1) * job_split * lightmap.width);
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
