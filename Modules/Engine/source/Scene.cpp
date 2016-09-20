// Scene.cpp

#include <Core/Memory/Functions.h>
#include "../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::Scene);

namespace sge
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
		for (const auto& componentType : _components)
		{
			for (auto component : componentType.second)
			{
				auto object = _component_objects.find(component.id);
				if (object != _component_objects.end())
				{
					if (componentType.first->has_drop())
					{
						componentType.first->drop(object->second);
					}

					sge::free(object->second);
				}
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
			return{ ComponentIdentity{ id, entity->second }, object->second };
		}
		else
		{
			return{ ComponentIdentity{ id, entity->second }, nullptr };
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
			return{ ComponentIdentity{ id, entity->second }, object->second };
		}
		else
		{
			return{ ComponentIdentity{ id, entity->second }, nullptr };
		}
	}

	ComponentInstance<void> Scene::new_component(EntityID entity, const TypeInfo& type, void* object)
	{
		// If the entity the component is being added to is the world entity or a non-existant entity, don't do anything
		if (entity == WORLD_ENTITY || _entity_parents.find(entity) == _entity_parents.end())
		{
			return ComponentInstance<void>::null();
		}

		// Create an identity for the component
		ComponentIdentity identity;
		identity.entity = entity;
		identity.id = _next_component_id++;
		_component_entities[identity.id] = entity;

		// If the type has a size, allocate memory for it
		void* buff = nullptr;
		if (type.size() > 0)
		{
			buff = sge::malloc(type.size());
			_component_objects[identity.id] = buff;

			if (object != nullptr)
			{
				type.move_init(buff, object);
			}
			else
			{
				type.init(object);
			}
		}

		// Find a spot for the identity in the array ordered by EntityID
		auto& typeArray = _components[&type];
		auto iter = typeArray.cbegin();
		while (iter != typeArray.cend() && iter->entity < entity)
		{
			++iter;
		}

		// Insert the identity
		typeArray.insert(iter, identity);

		// Give the componet the 'NewComponent' tag, so that it may be picked up by relevant systems
		//new_tag(identity.component, TNewComponent{});

		return{ identity, buff };
	}
}
