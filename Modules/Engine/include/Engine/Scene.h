// Scene.h
#pragma once

#include <map>
#include <vector>
#include <functional>
#include "Component.h"

namespace sge
{
	struct SGE_ENGINE_API Scene
	{
		SGE_REFLECTED_TYPE;
		
		typedef EntityID(SGE_C_CALL*SelectorFn)(void* outComponent, Scene& scene, int i);
		typedef void(SGE_C_CALL*ProcessorFn)(void* data, EntityID entity, const void** components);

		////////////////////////
		///   Constructors   ///
	public:

		Scene();
		Scene(const Scene& copy) = delete;
		Scene& operator=(const Scene& copy) = delete;
		Scene(Scene&& move) = default;
		Scene& operator=(Scene&& move) = default;
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
		ComponentInstance<T> new_component(EntityID entity, T&& component)
		{
			auto instance = this->new_component(entity, sge::get_type(component), &component);
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

		void new_tag(ComponentID component, const TypeInfo& type, void* tag);

		template <typename T>
		void new_tag(ComponentID component, T&& tag)
		{
			new_tag(component, sge::get_type(tag), &tag);
		}

		template <class System, typename ... Selectors>
		void new_system(System& system, void(System::*handler)(EntityID, Selectors...))
		{
			
		}

	private:
		
		template <typename ComponentType, typename ... Tags, typename ... RestS>
		static SelectorFn new_selectors(stde::type_sequence<RequiredComponent<ComponentType, Tags...>, RestS...>)
		{
			return [](void* outComponent, Scene& scene, int i) -> EntityID {
				
			};
		}

		////////////////
		///   Data   ///
	private:

		ComponentID _next_component_id;
		EntityID _next_entity_id;

		/* Component Data */
		std::map<ComponentID, EntityID> _component_entities;
		std::map<ComponentID, void*> _component_objects;
		std::map<ComponentID, std::map<const TypeInfo*, std::vector<void*>>> _component_tags;
		std::map<const TypeInfo*, std::vector<ComponentIdentity>> _components;

		/* System data */
		std::vector<std::function<void(Scene&)>> _systems;

		/* Entity Data */
		std::map<EntityID, EntityID> _entity_parents;
		std::map<EntityID, std::string> _entity_names;
	};
}
