// Scene.cpp

#include "../include/Engine/Scene.h"

REFLECT_TYPE(singe::Scene);

namespace singe
{
	////////////////////////
	///   Constructors   ///
	
	Scene::Scene()
	{
		_next_component_id = 1;
		_next_entity_id = 2;
	}

	Scene::~Scene()
	{
		for (auto componentType : _component_types)
		{
			auto object = _component_objects.find(componentType.first);
			if (object != _component_objects.end())
			{
				if (componentType.second->drop != nullptr)
				{
					componentType.second->drop(object->second);
				}

				std::free(object->second);
			}
		}
	}

	///////////////////
	///   Methods   ///

	EntityID Scene::new_entity()
	{
		auto id = _next_entity_id++;
		_entity_parents[id] = WORLD_ENTITY;

		return id;
	}

	EntityID Scene::get_entity_parent(EntityID entity) const
	{
		auto iter = _entity_parents.find(entity);
		if (iter != _entity_parents.end())
		{
			return iter->second;
		}
		else
		{
			return NULL_ENTITY;
		}
	}

	void Scene::set_entity_parent(EntityID entity, EntityID parent)
	{
		auto iter = _entity_parents.find(entity);
		if (iter != _entity_parents.end() && _entity_parents.find(parent) != _entity_parents.end())
		{
			iter->second = parent || WORLD_ENTITY;
		}
	}

	std::string Scene::get_entity_name(EntityID entity) const
	{
		if (entity == WORLD_ENTITY)
		{
			return "World";
		}

		auto iter = _entity_names.find(entity);
		if (iter != _entity_names.end())
		{
			return iter->second;
		}
		else
		{
			return "";
		}
	}

	void Scene::set_entity_name(EntityID entity, std::string name)
	{
		if (_entity_parents.find(entity) == _entity_parents.end())
		{
			return;
		}

		_entity_names[entity] = std::move(name);
	}

	ComponentInstance<void> Scene::get_component(ComponentID id)
	{
		auto entity = _component_entities.find(id);
		if (entity == _component_entities.end())
		{
			return ComponentInstance<void>::null();
		}

		auto object = _component_objects.find(id);
		if (object == _component_objects.end())
		{
			return{ id, entity->second, object->second };
		}
		else
		{
			return{ id, entity->second, nullptr };
		}
	}

	ComponentInstance<const void> Scene::get_component(ComponentID id) const
	{
		auto entity = _component_entities.find(id);
		if (entity == _component_entities.end())
		{
			return ComponentInstance<const void>::null();
		}

		auto object = _component_objects.find(id);
		if (object != _component_objects.end())
		{
			return{ id, entity->second, object->second };
		}
		else
		{
			return{ id, entity->second, nullptr };
		}
	}

	ComponentInstance<void> Scene::new_component(EntityID entity, const TypeInfo& type, void* object)
	{
		// If the entity the component is being added to is the world entity or a non-existant entity, don't do anything
		if (entity == WORLD_ENTITY || _entity_parents.find(entity) == _entity_parents.end())
		{
			return ComponentInstance<void>::null();
		}

		auto id = _next_component_id++;
		_component_types[id] = &type;
		_component_entities[id] = entity;
		
		// If the type has a size, allocate memory for it
		void* buff = nullptr;
		if (type.size != 0)
		{
			buff = std::malloc(type.size);
			_component_objects[id] = buff;
		}

		// If an initial state was passed it, move from that
		if (object != nullptr)
		{
			type.move_initialize(buff, object);
		}
		else
		{
			type.initialize(buff);
		}
		
		return{ id, entity, buff };
	}

}
