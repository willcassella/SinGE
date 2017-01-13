// EditorOps.cpp

#include <iostream>
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
#include <Engine/Scene.h>
#include <Engine/SystemFrame.h>

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

			void new_entity_query(SystemFrameMut& frame, ArchiveReader& reader)
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
					auto entity = frame.create_entity();
					std::cout << "Created entity " << entity << std::endl;
				}
			}

			void destroy_entity_query(SystemFrameMut& frame, ArchiveReader& reader)
			{
				// Enumerate entities to destroy
				reader.enumerate_array_elements([&frame, &reader](std::size_t /*i*/)
				{
					// Get the entity id
					EntityId entity = NULL_ENTITY;
					sge::from_archive(entity, reader);

					// Destroy it
					std::cout << "Destroying entity " << entity << std::endl;
					frame.destroy_entity(entity);
				});
			}

			void set_entity_name_query(SystemFrameMut& frame, ArchiveReader& reader)
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

			void set_entity_parent_query(SystemFrameMut& frame, ArchiveReader& reader)
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
					frame.set_entity_parent(entity_id, parent_id);
					std::cout << "Setting parent on entity " << entity_id << " to " << parent_id << std::endl;
				});
			}

			void new_component_query(SystemFrameMut& frame, ArchiveReader& reader)
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
						const auto* type = frame.get_scene().get_component_type(component_type.c_str());
						if (!type)
						{
							return;
						}

						// Create the component
						frame.new_component(entity_id, *type, nullptr);
						std::cout << "Created '" << type->name() << "' component on entity " << entity_id << std::endl;
					});
				});
			}

			void destroy_component_query(SystemFrameMut& frame, ArchiveReader& reader)
			{
				// Enumerate componen types to destroy
				reader.enumerate_object_members([&frame, &reader](const char* component_type_name)
				{
					// Get the component type
					const auto* type = frame.get_scene().get_component_type(component_type_name);
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
						frame.destroy_component(entity, *type);
					});
				});
			}

			void get_scene_query(const SystemFrame& frame, ArchiveWriter& writer)
			{
				const auto& scene_data = frame.get_scene().get_raw_scene_data();
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
					// Skip this component if there are not entities
					if (component_type.second.instances.empty())
					{
						continue;
					}

					// Enumerate instances
					writer.push_object_member(component_type.first->name().c_str());
					for (auto instance : component_type.second.instances)
					{
						writer.array_element(instance);
					}
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
					const auto* type = frame.get_scene().get_component_type(typeName);
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
						frame.process_single(entity_id, &type, 1, [type, &writer](ProcessingFrame&, EntityId entity, auto comp) -> ProcessControl
						{
							std::cout << "Reading properties of '" << type->name() << "' component on entity '" << entity << "'" << std::endl;
							ArchiveWriter* writers[] = { &writer };
							read_properties(Any<>{ *type, comp[0] }, writers, 1);
							return ProcessControl::BREAK;
						});

						writer.pop();
					});

					writer.pop();
				});
			}

			void set_component_query(SystemFrameMut& frame, ArchiveReader& reader)
			{
				// Enumerate types of components changed
				reader.enumerate_object_members([&frame, &reader](const char* typeName)
				{
					auto* type = frame.get_scene().get_component_type(typeName);
					if (!type)
					{
						return;
					}

					// Enumerate the EntityIds of components changed
					reader.enumerate_object_members([type, &frame, &reader](const char* entityId)
					{
						// Get the component Id
						ComponentId id{ std::strtoull(entityId, nullptr, 10), *type };

						// Process the component and deserialize it
						frame.process_single_mut(id.entity(), &type, 1, [type, &reader](ProcessingFrame&, EntityId entity, auto comp) -> ProcessControl
						{
							std::cout << "Writing properties of '" << type->name() << "' component on entity '" << entity << "'" << std::endl;
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
		}
	}
}
