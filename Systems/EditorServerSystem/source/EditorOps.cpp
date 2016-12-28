// EditorOps.cpp

#include <Core/IO/ArchiveWriter.h>
#include <Core/IO/ArchiveReader.h>
#include <Core/Reflection/Reflection.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/Reflection/TypeDB.h>
#include <Core/Reflection/PropertyInfo.h>
#include <Core/Interfaces/IToString.h>
#include <Engine/Scene.h>
#include <iostream>

namespace sge
{
	namespace editor_ops
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

		static void read_properties(Any<> object, ArchiveReader& reader, ArchiveWriter& writer)
		{
			reader.enumerate_object_members([&reader, &writer, object](const char* propName)
			{
				// Search for the property
				const auto* propInfo = object.type().find_property(propName);
				if (!propInfo)
				{
					return;
				}

				// Access the value of the property
				propInfo->get(object.object(), [&reader, &writer, propName](Any<> prop)
				{
					// If there are specific properties within this property we're querying for
					if (!reader.null())
					{
						writer.push_object_member(propName);
						read_properties(prop, reader, writer);
						writer.pop();
						return;
					}

					// Get the IToArchive implementation
					const auto* const impl = sge::get_vtable<IToArchive>(prop.type());
					if (!impl)
					{
						return;
					}

					writer.push_object_member(propName);
					impl->to_archive(prop.object(), writer);
					writer.pop();
				});
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

		void get_type_info_query(const Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
		{
			// Enumerate the types to get the properties from
			reader.enumerate_array_elements([&scene, &reader, &writer](std::size_t /*i*/)
			{
				// Get the type name
				std::string typeName;
				sge::from_archive(typeName, reader);

				// Search for the type
				const auto* type = scene.get_type_db().find_type(typeName.c_str());

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
			scene.enumerate_component_types([&writer](const TypeInfo& type)
			{
				writer.array_element(type.name());
			});
		}

		void get_component_query(const Scene& scene, ArchiveReader& reader, ArchiveWriter& writer)
		{
			// Enumerate the types to get the properties from
			reader.enumerate_object_members([&scene, &reader, &writer](const char* typeName)
			{
				writer.push_object_member(typeName);

				// Get the type
				const auto* type = scene.get_component_type(typeName);
				if (!type)
				{
					return;
				}

				// Enumerate the instances
				reader.enumerate_object_members([type, &scene, &reader, &writer](const char* idStr)
				{
					writer.push_object_member(idStr);

					// Get the EntityID
					EntityId entity = std::strtoull(idStr, nullptr, 10);
					scene.process_single(entity, &type, 1, [type, &reader, &writer](
						ProcessingFrame&,
						EntityId,
						const ComponentInterface* const components[])
					{
						editor_ops::read_properties(Any<>{ *type, components[0] }, reader, writer);
					});

					writer.pop();
				});

				writer.pop();
			});
		}

		void set_component_query(Scene& scene, ArchiveReader& reader)
		{
			// Enumerate types of components changed
			reader.enumerate_object_members([&scene, &reader](const char* typeName)
			{
				auto* type = scene.get_component_type(typeName);
				if (!type)
				{
					return;
				}

				// Enumerate the EntityIds of components changed
				reader.enumerate_object_members([type, &scene, &reader](const char* entityId)
				{
					// Get the component Id
					ComponentId id{ std::strtoull(entityId, nullptr, 10), *type };

					// Process the component and deserialize it
					scene.process_single_mut(id.entity(), &type, 1, [&reader](ProcessingFrame&, EntityId, auto comp)
					{
						write_properties(AnyMut<>{ comp[0]->get_type(), comp[0] }, reader);
					});
				});
			});
		}
	}
}
