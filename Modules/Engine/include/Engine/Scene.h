// Scene.h
#pragma once

#include <map>
#include <vector>
#include "Component.h"

namespace singe
{
	struct ENGINE_API Scene final
	{
		REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		Scene();
		Scene(const Scene& copy) = delete;
		~Scene();

		///////////////////
		///   Methods   ///
	public:

		/* Creates a new Entity. */
		EntityID new_entity();

		/* Returns the parent of the given Entity. */
		EntityID get_entity_parent(EntityID entity) const;

		void set_entity_parent(EntityID entity, EntityID parent);

		std::string get_entity_name(EntityID entity) const;

		void set_entity_name(EntityID entity, std::string name);
		
		ComponentInstance<void> new_component(EntityID entity, const TypeInfo& type, void* object);

		template <typename T>
		ComponentInstance<T> new_component(EntityID entity, T component)
		{
			auto instance = this->new_component(entity, get_type<T>(), &component);
			return{ instance.id, instance.entity, static_cast<T*>(instance.object) };
		}

		ComponentInstance<void> get_component(ComponentID id);

		template <typename T>
		ComponentInstance<T> get_component(Handle<T> handle)
		{
			auto instance = this->get_component(handle.id);
			return{ instance.id, instance.entity, static_cast<T*>(instance.object) };
		}

		ComponentInstance<const void> get_component(ComponentID id) const;

		template <typename T>
		ComponentInstance<const T> get_component(Handle<T> handle) const
		{
			auto instance = this->get_component(handle.id);
			return{ instance.id, instance.entity, static_cast<const T*>(instance.object) };
		}

		template <typename T>
		auto enumerate_components() const
		{
			std::vector< ComponentInstance<const T> > result;
			for (auto componentType : _component_types)
			{
				if (componentType.second == &get_type<T>())
				{
					Handle<T> handle;
					handle.id = componentType.first;
					result.push_back(get_component(handle));
				}
			}

			return result;
		}

		////////////////
		///   Data   ///
	private:

		ComponentID _next_component_id;
		EntityID _next_entity_id;

		/* Component Data */
		std::map<ComponentID, EntityID> _component_entities;
		std::map<ComponentID, const TypeInfo*> _component_types;
		std::map<ComponentID, void*> _component_objects;

		/* Entity Data */
		std::map<EntityID, EntityID> _entity_parents;
		std::map<EntityID, std::string> _entity_names;
	};
}
