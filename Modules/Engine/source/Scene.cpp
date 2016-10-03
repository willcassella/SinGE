// Scene.cpp

#include <Core/Memory/Functions.h>
#include <Core/Functional/FunctionView.h>
#include "../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::Scene);

namespace sge
{
	////////////////////////
	///   Constructors   ///

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

	void Scene::run_system(FunctionView<SystemFnMut> system, const TypeInfo** types, std::size_t nTypes)
	{
		if (nTypes == 0)
		{
			return;
		}

		auto primaryType = _components.find(types[0]);
		if (primaryType == _components.end())
		{
			return;
		}

		// Create an array to hold the selected components
		auto* results = SGE_STACK_ALLOC(ComponentInstanceMut, nTypes);

		// Iter through all entities that the primary component type appears on
		for (EntityId entity : primaryType->second)
		{
			bool satisfied = true;
			for (std::size_t i = 1; i < nTypes; ++i)
			{
				// If this type does not exist in the scene OR this entity does not have an instance of it
				auto iter = _components.find(types[i]);
				if (iter == _components.end() || iter->second.find(entity) == iter->second.end())
				{
					satisfied = false;
					break;
				}

				ComponentId id{ entity, *types[i] };
				new (results + i) ComponentInstanceMut{ id, get_component_object(id) };
			}

			// If all further requirements were satisfied
			if (satisfied)
			{
				// Fill in the primary object
				ComponentId primaryId{ entity, *types[0] };
				new (results) ComponentInstanceMut{ primaryId, get_component_object(primaryId) };

				// Call the system function
				Frame frame{ *this, _current_time };
				system(frame, entity, results);
			}
		}
	}

	void Scene::run_system(FunctionView<SystemFn> system, const TypeInfo** types, std::size_t nTypes) const
	{
		// TODO
	}

	void* Scene::get_component_object(ComponentId id) const
	{
		auto iter = _component_objects.find(id);
		return iter != _component_objects.end() ? iter->second : nullptr;
	}
}
