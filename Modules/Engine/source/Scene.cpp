// Scene.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Memory/Functions.h>
#include <Core/Functional/FunctionView.h>
#include "../include/Engine/Scene.h"
#include "Core/Util/StringUtils.h"

SGE_REFLECT_TYPE(sge::Scene)
.implements<IToArchive>()
.implements<IFromArchive>();

namespace sge
{
	Scene::Scene()
	{
		_current_time = 0;
		_next_entity_id = 2; // '1' is reserved for WORLD_ENTITY
	}

	Scene::~Scene()
	{
		for (auto component : _component_objects)
		{
			component.first.type()->drop(component.second);
			sge::free(component.second);
		}
	}

	///////////////////
	///   Methods   ///

	void Scene::to_archive(ArchiveWriter& writer) const
	{
		// Serialize next entity id
		writer.push_object_member("next_entity_id", _next_entity_id);

		// Serialize all entities
		writer.add_object_member("entities", [&](ArchiveWriter& entityListWriter)
		{
			for (auto entity : this->_entity_parents)
			{
				entityListWriter.add_object_member(
					to_string(entity.first).c_str(),
					[&](ArchiveWriter& entityWriter)
				{
					// Write the entity id and parent id
					entityWriter.push_object_member("parent", entity.second);

					// See if the entity has a name
					auto nameIter = this->_entity_names.find(entity.first);
					if (nameIter != this->_entity_names.end())
					{
						entityWriter.push_object_member("name", nameIter->second);
					}
				});
			}
		});

		// Serialize all components
		writer.add_object_member("components", [&](ArchiveWriter& componentListWriter)
		{
			// For each type of component
			for (const auto& componentType : this->_components)
			{
				// Add the component type name as a field
				componentListWriter.add_object_member(componentType.first->name().c_str(), [&](ArchiveWriter& componentTypeWriter)
				{
					// Get the vtable for the 'ToArchive' interface
					const auto* toArchive = get_vtable<IToArchive>(*componentType.first);

					// Serialize each instance of the component
					for (auto componentEntity : componentType.second)
					{
						componentTypeWriter.add_object_member(
							to_string(componentEntity).c_str(),
							[&](ArchiveWriter& componentWriter)
						{
							const auto* object = this->get_component_object(ComponentId{ componentEntity, *componentType.first });
							toArchive->to_archive(object, componentWriter);
						});
					}
				});
			}
		});
	}

	void Scene::from_archive(const ArchiveReader& reader)
	{
		decltype(_entity_parents) entities;
		decltype(_entity_names) entityNames;

		// Deserialize all entities
		reader.object_member("entities", [&](const ArchiveReader& entityListReader)
		{
			// Get the number of entities up front
			std::size_t numEntities = 0;
			entityListReader.object_num_members(numEntities);
			entities.reserve(numEntities);

			// Load all entities
			entityListReader.enumerate_object_members([&](const char* id, const ArchiveReader& entityReader)
			{
				// Get the entities ID and Parent
				EntityId entity = NULL_ENTITY, parent = WORLD_ENTITY;
				entity = std::strtoull(id, nullptr, 10);
				entityReader.pull_object_member("parent", parent);

				// Make sure the entity fields are valid
				if (entity == NULL_ENTITY || entity == WORLD_ENTITY || parent == NULL_ENTITY)
				{
					return;
				}

				// Add the entity to the world
				entities.insert(std::make_pair(entity, parent));

				// Get the entity's name
				std::string name;
				if (entityReader.pull_object_member("name", name))
				{
					entityNames.insert(std::make_pair(entity, std::move(name)));
				}
			});
		});

		// Validate entity-parent relationships
		for (auto entity : entities)
		{
			// World entity is a valid parent
			if (entity.second == WORLD_ENTITY)
			{
				continue;
			}

			// Otherwise, the parent has to be a valid entity
			auto iter = entities.find(entity.second);
			if (iter == entities.end())
			{
				return;
			}
		}

		decltype(_components) components;
		decltype(_component_objects) componentObjects;

		// Deserialize all components
		reader.object_member("components", [&](const ArchiveReader& componentListReader)
		{
			// Enumerate all types of components
			componentListReader.enumerate_object_members([&](const char* name, const ArchiveReader& componentTypeReader)
			{
				// Try to get the component type
				auto typeIter = _component_types.find(name);
				if (typeIter == _component_types.end())
				{
					return;
				}

				// Get the vtable for 'FromArchive' for this type
				const auto* fromArchive = get_vtable<IFromArchive>(*typeIter->second);

				// Create containers for instances and objects
				std::set<EntityId> instanceEntities;

				// Load all instances
				componentTypeReader.enumerate_object_members([&](const char* entity, const ArchiveReader& componentReader)
				{
					// Get the entity id
					EntityId entityId = NULL_ENTITY;
					entityId = std::strtoull(entity, nullptr, 10);

					// Validate the entity id (it may not be the null entity, world entity, or an entity that already exists for this type)
					if (entityId == NULL_ENTITY ||
						entityId == WORLD_ENTITY ||
						instanceEntities.find(entityId) != instanceEntities.end())
					{
						return;
					}

					// Add the entity to the set
					instanceEntities.insert(entityId);

					// Create an instance of the entity
					auto* buff = std::malloc(typeIter->second->size());
					typeIter->second->init(buff);

					// Add the component object to the scene
					componentObjects.insert(std::make_pair(ComponentId{ entityId, *typeIter->second }, buff));

					// Deserialize it
					fromArchive->from_archive(buff, componentReader);
				});

				// Insert the entity set
				components.insert(std::make_pair(typeIter->second, std::move(instanceEntities)));
			});
		});

		// Validate component entities
		for (const auto& componentType : components)
		{
			for (auto entity : componentType.second)
			{
				// Make sure the entity actually exists
				auto iter = entities.find(entity);
				if (iter == entities.end())
				{
					return;
				}
			}
		}

		// Add data to the scene
		_entity_parents = std::move(entities);
		_entity_names = std::move(entityNames);
		_components = std::move(components);
		_component_objects = std::move(componentObjects);

		reader.pull_object_member("next_entity_id", _next_entity_id);
	}

	void Scene::register_component_type(const TypeInfo& type)
	{
		_component_types.insert(std::make_pair(type.name(), &type));
	}

	EntityId Scene::new_entity()
	{
		auto id = _next_entity_id++;
		_entity_parents[id] = WORLD_ENTITY;

		return id;
	}

	EntityId Scene::get_entity_parent(EntityId entity) const
	{
		auto iter = _entity_parents.find(entity);
		return iter != _entity_parents.end() ? iter->second : NULL_ENTITY;
	}

	void Scene::set_entity_parent(EntityId entity, EntityId parent)
	{
		// Make sure the target entity actually exists
		auto iter = _entity_parents.find(entity);
		if (iter == _entity_parents.end())
		{
			return;
		}

		if (parent == NULL_ENTITY || parent == WORLD_ENTITY)
		{
			iter->second = WORLD_ENTITY;
			return;
		}

		// Make sure the parent entity actually exists
		auto parentIter = _entity_parents.find(parent);
		if (parentIter == _entity_parents.end())
		{
			return;
		}

		// TODO: Check for entity parent cycle
		iter->second = parent;
	}

	std::string Scene::get_entity_name(EntityId entity) const
	{
		if (entity == WORLD_ENTITY)
		{
			return "World";
		}

		auto iter = _entity_names.find(entity);
		return iter != _entity_names.end() ? iter->second : "";
	}

	void Scene::set_entity_name(EntityId entity, std::string name)
	{
		if (_entity_parents.find(entity) == _entity_parents.end())
		{
			return;
		}

		_entity_names[entity] = std::move(name);
	}

	ComponentInstanceMut Scene::new_component(EntityId entity, const TypeInfo& type, void* object)
	{
		// If the entity the component is being added to is the world entity or a non-existant entity, don't do anything
		if (entity == NULL_ENTITY || entity == WORLD_ENTITY || _entity_parents.find(entity) == _entity_parents.end())
		{
			return ComponentInstanceMut::null();
		}

		ComponentId id{ entity, type };

		// If this component already exists in the world
		if (component_exists(id))
		{
			return get_component(id);
		}

		// Add this component to the entity
		_components[&type].insert(entity);

		// If the type has a size, allocate memory for it
		void* buff = nullptr;
		if (!type.is_empty())
		{
			buff = sge::malloc(type.size());
			_component_objects[id] = buff;

			if (object != nullptr)
			{
				type.move_init(buff, object);
			}
			else
			{
				type.init(buff);
			}
		}

		return{ id, buff };
	}

	bool Scene::component_exists(ComponentId id) const
	{
		// Make sure the the type of this component actually exists in the scene
		auto set = _components.find(id.type());
		if (set == _components.end())
		{
			return false;
		}

		// Make sure a component of that type is actually attached to the given entity
		auto entity = set->second.find(id.entity());
		if (entity == set->second.end())
		{
			return false;
		}

		return true;
	}

	ComponentInstanceMut Scene::get_component(ComponentId id)
	{
		if (!component_exists(id))
		{
			return ComponentInstanceMut::null();
		}

		// Get the object (if applicable) for this component
		auto object = _component_objects.find(id);
		if (object == _component_objects.end())
		{
			return{ id, nullptr };
		}

		return{ id, object->second };
	}

	ComponentInstance Scene::get_component(ComponentId id) const
	{
		if (!component_exists(id))
		{
			return ComponentInstanceMut::null();
		}

		// Get the object (if applicable) for this component
		auto object = _component_objects.find(id);
		if (object == _component_objects.end())
		{
			return{ id, nullptr };
		}

		return{ id, object->second };
	}

	void Scene::run_system(FunctionView<SystemFnMut> system, const TypeInfo** types, std::size_t numTypes)
	{
		impl_run_system<ComponentInstanceMut>(*this, system, types, numTypes);
	}

	void Scene::run_system(FunctionView<SystemFn> system, const TypeInfo** types, std::size_t numTypes) const
	{
		impl_run_system<ComponentInstance>(*this, system, types, numTypes);
	}

	void* Scene::get_component_object(ComponentId id) const
	{
		auto iter = _component_objects.find(id);
		return iter != _component_objects.end() ? iter->second : nullptr;
	}

	template <typename InstanceT, typename SelfT, typename SystemT>
	void Scene::impl_run_system(SelfT& self, SystemT system, const TypeInfo** types, std::size_t numTypes)
	{
		if (numTypes == 0)
		{
			return;
		}

		auto primaryType = self._components.find(types[0]);
		if (primaryType == self._components.end())
		{
			return;
		}

		// Create an array to hold the selected components
		auto* results = SGE_STACK_ALLOC(InstanceT, numTypes);

		// Iter through all entities that the primary component type appears on
		for (EntityId entity : primaryType->second)
		{
			bool satisfied = true;
			for (std::size_t i = 1; i < numTypes; ++i)
			{
				// If this type does not exist in the scene OR this entity does not have an instance of it
				auto iter = self._components.find(types[i]);
				if (iter == self._components.end() || iter->second.find(entity) == iter->second.end())
				{
					satisfied = false;
					break;
				}

				ComponentId id{ entity, *types[i] };
				new (results + i) InstanceT{ id, self.get_component_object(id) };
			}

			// If all further requirements were satisfied
			if (satisfied)
			{
				// Fill in the primary object
				ComponentId primaryId{ entity, *types[0] };
				new (results) InstanceT{ primaryId, self.get_component_object(primaryId) };

				// Call the system function
				Frame frame{ self, self._current_time };
				system(frame, entity, results);
			}
		}
	}
}
