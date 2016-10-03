// Scene.h
#pragma once

#include <set>
#include <unordered_map>
#include "Frame.h"
#include "Component.h"

namespace sge
{
	struct SGE_ENGINE_API Scene
	{
		SGE_REFLECTED_TYPE;

		using SystemFnMut = void(Frame& frame, EntityId entity, const ComponentInstanceMut* components);
		using SystemFn = void(const Frame& frame, EntityId entity, const ComponentInstance* components);

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
		EntityId new_entity();

		/* Returns the parent of the given Entity. */
		EntityId get_entity_parent(EntityId entity) const;

		void set_entity_parent(EntityId entity, EntityId parent);

		std::string get_entity_name(EntityId entity) const;

		void set_entity_name(EntityId entity, std::string name);

		/* Creates a new instance of the given type of Component, optionally moving from the provided initial value. */
		ComponentInstanceMut new_component(EntityId entity, const TypeInfo& type, void* init);

		/* Creates a new instance of the given type of Component, moving from the provided initial value. */
		template <typename T>
		TComponentInstance<T> new_component(EntityId entity, T&& component)
		{
			auto instance = this->new_component(entity, sge::get_type(component), &component);
			return{ instance.entity(), static_cast<T*>(instance.object()) };
		}

		/* Default-constructs a new instance of the given type of Component. */
		template <typename T>
		TComponentInstance<T> new_component(EntityId entity)
		{
			auto instance = this->new_component(entity, sge::get_type<T>(), nullptr);
			return{ instance.entity(), static_cast<T*>(instance.object()) };
		}

		bool component_exists(ComponentId id) const;

		ComponentInstanceMut get_component(ComponentId id);

		template <typename T>
		TComponentInstance<T> get_component(TComponentId<T> id)
		{
			auto instance = this->get_component(static_cast<ComponentId>(id));
			return{ instance.entity(), static_cast<T*>(instance.object()) };
		}

		ComponentInstance get_component(ComponentId id) const;

		template <typename T>
		TComponentInstance<const T> get_component(TComponentId<T> id) const
		{
			auto instance = this->get_component(static_cast<ComponentId>(id));
			return{ instance.entity(), static_cast<const T*>(instance.object()) };
		}

		/**
		 * \brief Runs the given system function on all Entities that meet the selection requirements.
		 * \param system Function to process the entity and components selected.
		 * \param types An array of required types.
		 * \param nTypes The number of types passed in.
		 */
		void run_system(FunctionView<SystemFnMut> system, const TypeInfo** types, std::size_t nTypes);

		void run_system(FunctionView<SystemFn> system, const TypeInfo** types, std::size_t nTypes) const;

		template <typename Ret, typename ... Components>
		void run_system(Ret(*system)(Frame&, EntityId, Components...))
		{
			// Create system wrapper function
			auto func = [system](Frame& frame, EntityId entity, const ComponentInstanceMut* components) {
				invoke_system(stde::type_sequence<Components...>{}, frame, entity, components, system);
			};

			// Create list of component types
			const TypeInfo* types[sizeof...(Components)];
			fill_types(stde::type_sequence<Components...>{}, types);

			// Run the system
			run_system(func, types, sizeof...(Components));
		}

		template <class System, typename Ret, typename ... Components>
		void run_system(System& system, void(System::*handler)(Frame&, EntityId, Components...))
		{
			// Create the system function
			auto invoker = [&system, handler](Frame& frame, EntityId entity, Components... components) {
				(system.*handler)(frame, entity, components...);
			};

			auto func = [&invoker](Frame& frame, EntityId entity, const ComponentInstanceMut* components) {
				invoke_system(stde::type_sequence<Components...>{}, frame, entity, components, invoker);
			};

			// Create the list of component types
			const TypeInfo* types[sizeof...(Components)];
			fill_types(stde::type_sequence<Components...>{}, types);

			// Run the system
			run_system(func, types, sizeof...(Components));
		}

	private:

		template <class C, typename ... Cs>
		static void fill_types(stde::type_sequence<TComponentInstance<C>, Cs...>, const TypeInfo** result)
		{
			*result = &sge::get_type<C>();
			fill_types(stde::type_sequence<Cs...>{}, result + 1);
		}

		static void fill_types(stde::type_sequence<>, const TypeInfo** /*result*/)
		{
		}

		template <typename SystemFn, class C, typename ... Cs, typename ... Converted>
		static void invoke_system(
			stde::type_sequence<TComponentInstance<C>, Cs...>,
			Frame& frame,
			EntityId entity,
			const ComponentInstanceMut* components,
			SystemFn system,
			const Converted&... converted)
		{
			invoke_system(
				stde::type_sequence<Cs...>{},
				frame,
				entity,
				components + 1,
				system,
				converted...,
				*static_cast<const TComponentInstance<C>*>(components));
		}

		template <typename SystemFn, typename ... Converted>
		static void invoke_system(
			stde::type_sequence<>,
			Frame& frame,
			EntityId entity,
			const ComponentInstanceMut* /*components*/,
			SystemFn system,
			const Converted&... converted)
		{
			system(frame, entity, converted...);
		}

		void* get_component_object(ComponentId id) const;

		////////////////
		///   Data   ///
	private:

		EntityId _next_entity_id;
		float _current_time;

		/* Entity Data */
		std::unordered_map<EntityId, EntityId> _entity_parents;
		std::unordered_map<EntityId, std::string> _entity_names;

		/* Component Data */
		std::unordered_map<ComponentId, void*> _component_objects;
		std::unordered_map<const TypeInfo*, std::set<EntityId>> _components;
	};
}
