// Scene.cpp

#include <Core/Reflection/TypeDB.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Memory/Functions.h>
#include <Core/Functional/FunctionView.h>
#include <Core/Util/StringUtils.h>
#include "../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::Scene)
.implements<IToArchive>()
.implements<IFromArchive>();

namespace sge
{
	Scene::Scene(TypeDB& typedb)
		: _type_db(&typedb)
	{
		_current_time = 0;
		_next_entity_id = 2; // '1' is reserved for WORLD_ENTITY
	}

	Scene::~Scene()
	{
	}

	///////////////////
	///   Methods   ///

	void Scene::to_archive(ArchiveWriter& writer) const
	{
		// Serialize next entity id
		writer.object_member("next_entity_id", _next_entity_id);

		// Serialize all entities
		writer.push_object_member("entities");
		for (auto entity : this->_entity_parents)
		{
			writer.push_object_member(to_string(entity.first).c_str());

			// Write the entity id and parent id
			writer.object_member("parent", entity.second);

			// See if the entity has a name
			auto nameIter = this->_entity_names.find(entity.first);
			if (nameIter != this->_entity_names.end())
			{
				writer.object_member("name", nameIter->second);
			}

			writer.pop();
		}
		writer.pop();

		// Serialize all components
		writer.push_object_member("components");
		for (const auto& componentType : this->_components)
		{
			// Add the component type name as a field
			writer.object_member(componentType.first->name().c_str(), *componentType.second.get());
		}
		writer.pop();
	}

	void Scene::from_archive(ArchiveReader& reader)
	{
		decltype(_entity_parents) entities;
		decltype(_entity_names) entityNames;

		// Deserialize entities
		reader.pull_object_member("entities");

		// Get the number of entities
		std::size_t numEntities = 0;
		reader.object_size(numEntities);
		entities.reserve(numEntities);

		// Load all entities
		reader.enumerate_object_members([&](const char* id)
		{
			// Get the entities ID and Parent
			EntityId entity = NULL_ENTITY, parent = WORLD_ENTITY;
			entity = std::strtoull(id, nullptr, 10);
			reader.object_member("parent", parent);

			// Make sure the entity fields are valid
			if (entity == NULL_ENTITY || entity == WORLD_ENTITY || parent == NULL_ENTITY)
			{
				return;
			}

			// Add the entity to the world
			entities.insert(std::make_pair(entity, parent));

			// Get the entity's name
			std::string name;
			if (reader.object_member("name", name))
			{
				entityNames.insert(std::make_pair(entity, std::move(name)));
			}
		});
		reader.pop();

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

		// Deserialize all components
		reader.pull_object_member("components");
		reader.enumerate_object_members([&](const char* name)
		{
			// Try to get the component type
			auto type = get_component_type(name);
			if (!type)
			{
				return;
			}

			// Clear the entities
			auto storageIter = _components.find(type);

			// Deserialize the storage object
			storageIter->second->from_archive(reader);
		});
		reader.pop();

		// Add data to the scene
		_entity_parents = std::move(entities);
		_entity_names = std::move(entityNames);

		reader.object_member("next_entity_id", _next_entity_id);
	}

	void Scene::register_component_type(const TypeInfo& type, std::unique_ptr<ComponentContainer> container)
	{
		_components.insert(std::make_pair(&type, std::move(container)));
		_type_db->new_type(type);
	}

	TypeDB& Scene::get_type_db() const
	{
		return *_type_db;
	}

	const TypeInfo* Scene::get_component_type(const char* typeName) const
	{
		auto type = _type_db->find_type(typeName);
		if (type && _components.find(type) != _components.end())
		{
			return type;
		}

		return nullptr;
	}

	void Scene::enumerate_component_types(FunctionView<ComponentTypeEnumeratorFn> enumerator) const
	{
		for (const auto& type : _components)
		{
			// Call the enumerator
			enumerator(*type.first);
		}
	}

	void Scene::enumerate_entities(FunctionView<EntityEnumeratorFn> enumerator) const
	{
		for (auto entity : _entity_parents)
		{
			enumerator(entity.first);
		}
	}

	void Scene::enumerate_components(EntityId entity, FunctionView<ComponentTypeEnumeratorFn> enumerator) const
	{
		for (const auto& type : _components)
		{
			if (type.second->entities.find(entity) != type.second->entities.end())
			{
				enumerator(*type.first);
			}
		}
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

	ComponentId Scene::new_component(EntityId entity, const TypeInfo& type)
	{
		// If the entity the component is being added to is the world entity or a non-existant entity, don't do anything
		if (entity == NULL_ENTITY || entity == WORLD_ENTITY || _entity_parents.find(entity) == _entity_parents.end())
		{
			return ComponentId::null();
		}

		ComponentId id{ entity, type };

		// Get the storage object
		auto& storage = _components[&type];

		// Create an instance of the component
		storage->create_component(entity);
		return id;
	}

	void Scene::process_entities_mut(const TypeInfo* const types[], std::size_t numTypes, FunctionView<ProcessMutFn> processFn)
	{
		impl_process_entities<ProcessingFrameMut>(*this, types, numTypes, processFn);
	}

	void Scene::process_entities(const TypeInfo* const types[], std::size_t numTypes, FunctionView<ProcessFn> processFn) const
	{
		impl_process_entities<ProcessingFrame>(*this, types, numTypes, processFn);
	}

	void Scene::process_single_mut(EntityId entity, const TypeInfo* const types[], std::size_t numTypes, FunctionView<ProcessMutFn> processFn)
	{
		impl_process_single<ProcessingFrameMut>(*this, entity, types, numTypes, processFn);
	}

	void Scene::process_single(EntityId entity, const TypeInfo* const types[], std::size_t numTypes, FunctionView<ProcessFn> processFn) const
	{
		impl_process_single<ProcessingFrame>(*this, entity, types, numTypes, processFn);
	}

	template <typename PFrameT, typename SelfT, typename ProcessFnT>
	void Scene::impl_process_entities(SelfT& self, const TypeInfo* const types[], std::size_t numTypes, ProcessFnT& processFn)
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

		// Create a processing frame
		PFrameT pframe{ self };

		// Create the array of component interfaces
		ComponentInterface** interfaceArray = SGE_STACK_ALLOC(ComponentInterface*, numTypes);

		// Fill the interface array
		for (std::size_t i = 0; i < numTypes; ++i)
		{
			interfaceArray[i] = reinterpret_cast<ComponentInterface*>(SGE_STACK_ALLOC(byte, types[i]->size()));
		}

		// Iterate through all entities that the primary component type appears on
		for (EntityId entity : primaryType->second->entities)
		{
			bool satisfied = true;
			for (std::size_t i = 1; i < numTypes; ++i)
			{
				// If this type does not exist in the scene
				auto iter = self._components.find(types[i]);
				if (iter == self._components.end())
				{
					satisfied = false;
					break;
				}

				// Try to create the component interface
				if (!iter->second->create_interface(pframe, entity, interfaceArray[i]))
				{
					satisfied = false;
					break;
				}
			}

			// If all further requirements were satisfied
			if (satisfied)
			{
				// Create the primary component interface
				primaryType->second->create_interface(pframe, entity, interfaceArray[0]);

				// Call the processing function
				processFn(pframe, entity, interfaceArray);
			}
		}
	}

	template <typename PFrameT, typename SelfT, typename ProcessFnT>
	void Scene::impl_process_single(SelfT& self, EntityId entity, const TypeInfo* const types[], std::size_t numTypes, ProcessFnT& processFn)
	{
		if (numTypes == 0)
		{
			return;
		}

		// Create a processing frame
		PFrameT pframe{ self };

		// Create the array of component interfaces
		ComponentInterface** interfaceArray = SGE_STACK_ALLOC(ComponentInterface*, numTypes);

		// Fill the interface array
		for (std::size_t i = 0; i < numTypes; ++i)
		{
			// Make sure this type is supported by the scene
			auto typeIter = self._components.find(types[i]);
			if (typeIter == self._components.end())
			{
				return;
			}

			// Allocate space for the component interface object
			interfaceArray[i] = reinterpret_cast<ComponentInterface*>(SGE_STACK_ALLOC(byte, types[i]->size()));

			// Try to get the component interface from the entity
			if (!typeIter->second->create_interface(pframe, entity, interfaceArray[i]))
			{
				return;
			}
		}

		// Call the processing function
		processFn(pframe, entity, interfaceArray);
	}
}
