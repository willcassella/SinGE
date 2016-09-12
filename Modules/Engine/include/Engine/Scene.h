// Scene.h
#pragma once

#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include "Component.h"

namespace sge
{
	struct Frame;

	struct SGE_ENGINE_API Scene
	{
		SGE_REFLECTED_TYPE;
		
		typedef EntityID(*SelectorFn)(void* outComponent, Scene& scene, int i);
		typedef void(*ProcessorFn)(void* data, EntityID entity, const void** components);

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
		
		/* Creates a new instance of the given type of Component, optionally moving from the provided initial value. */
		ComponentInstance<void> new_component(EntityID entity, const TypeInfo& type, void* init);
		
		/* Creates a new instance of the given type of Component, moving from the provided initial value. */
		template <typename T>
		ComponentInstance<T> new_component(EntityID entity, T&& component)
		{
			auto instance = this->new_component(entity, sge::get_type(component), &component);
			return{ instance.id, instance.entity, static_cast<T*>(instance.object) };
		}

		/* Default-constructs a new instance of the given type of Component. */
		template <typename T>
		ComponentInstance<T> new_component(EntityID entity)
		{
			auto instance = this->new_component(entity, sge::get_type<T>(), nullptr);
			return{ intstance.id, intsance.entity, static_cast<T*>(instance.object) };
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

		template <class System, typename ... Selectors>
		void run_system(System& system, void(System::*handler)(Scene&, EntityID, Selectors...))
		{
			auto func = [&](Scene& scene, EntityID entity, Selectors... selectors) {
				(system.*handler)(scene, entity, selectors...);
			};
			select(stde::type_sequence<Selectors...>{}, func, WORLD_ENTITY);
		}

	private:

		template <typename F, typename C, typename ... RestS, typename ... PrevS>
		bool select(stde::type_sequence<comp::With<C>, RestS...>, F f, EntityID entity, PrevS ... prev)
		{
			constexpr bool IS_PRIMARY_SELECTOR = sizeof...(PrevS) == 0;
			for (auto instance : _components[&sge::get_type<C>()])
			{
				// If we've passed the entity
				if (instance.entity > entity)
				{
					// If we're the first selector
					if (IS_PRIMARY_SELECTOR)
					{
						entity = instance.entity;
					}
					else
					{
						// Can't invoke for this one, but we can keep searching
						return true;
					}
				}

				// Entity found, construct the instance
				if (instance.entity == entity)
				{
					comp::With<C> result{
						instance.id,
						instance.entity,
						static_cast<C*>(_component_objects[instance.id])
					};

					bool cont = select(stde::type_sequence<RestS...>{}, f, entity, prev..., result);
					if (!IS_PRIMARY_SELECTOR)
					{
						return cont;
					}
				}
			}

			// No more components of this type, so stop search
			return false;
		}

		template <typename F, typename C, typename ... RestS, typename ... PrevS>
		bool select(stde::type_sequence<comp::Optional<C>, RestS...>, F f, EntityID entity, PrevS... prev)
		{
			static_assert(sizeof...(PrevS) != 0, "You you may not use 'Optional' as the primary selector.");
			for (auto instance : _components[&sge::get_type<C>()])
			{
				// If the component exists for this entity
				if (instance.entity == entity)
				{
					OptionalComponent<C> result{
						instance.id,
						instance.entity,
						static_cast<C*>(_component_objects[instance.id])
					};

					return select(stde::type_sequence<RestS...>{}, f, entity, prev..., result);
				}
			}

			comp::Optional<C> result{
				NULL_COMPONENT,
				NULL_ENTITY,
				nullptr
			};
			return select(stde::type_sequence<RestS...>{}, f, entity, prev..., result);
		}

		template <typename F, typename C, typename ... RestS, typename ... PrevS>
		bool select(stde::type_sequence<comp::Without<C>>, F f, EntityID entity, PrevS ... prev)
		{
			static_assert(sizeof...(PrevS) != 0, "You may not use 'Wihout' as the primary selector.");
			for (auto instance : _components[&sge::get_type<C>()])
			{
				// If the component exists for this entity
				if (instance.entity == entity)
				{
					return true;
				}
			}

			return select(stde::type_sequence<RestS...>{}, f, entity, prev..., comp::Without<C>{});
		}

		template <typename F, typename ... Selectors>
		bool select(stde::type_sequence<>, F f, EntityID entity, Selectors... selectors)
		{
			f(*this, entity, selectors...);
			return true;
		}

		////////////////
		///   Data   ///
	private:

		ComponentID _next_component_id;
		EntityID _next_entity_id;

		/* Component Data */
		std::map<ComponentID, EntityID> _component_entities;
		std::map<ComponentID, void*> _component_objects;
		std::map<const TypeInfo*, std::vector<ComponentIdentity>> _components;

		/* Entity Data */
		std::map<EntityID, EntityID> _entity_parents;
		std::map<EntityID, std::string> _entity_names;
	};
}
