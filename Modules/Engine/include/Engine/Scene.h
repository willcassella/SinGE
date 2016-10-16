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
		void run_system(FunctionView<SystemFnMut> system, const TypeInfo** types, std::size_t numTypes);

		void run_system(FunctionView<SystemFn> system, const TypeInfo** types, std::size_t numTypes) const;

		template <typename SystemFn>
		void run_system(SystemFn& lambda)
		{
			using FnTraits = stde::function_traits<SystemFn>;
			using component_list = tmp::cdr<tmp::cdr<typename FnTraits::arg_types>>;

			// Run the system
			static_run_system(component_list{}, *this, lambda);
		}

		template <typename SystemFn>
		void run_system(SystemFn& lambda) const
		{
			using FnTraits = stde::function_traits<SystemFn>;
			using component_list = tmp::cdr<tmp::cdr<typename FnTraits::arg_types>>;

			// Run the system
			static_run_system(component_list{}, *this, lambda);
		}

		template <class System, typename Ret, typename ... Components>
		void run_system(System& system, Ret(System::*func)(Frame&, EntityId, Components...))
		{
			// Create a wrapper function
			auto wrapper = [&system, func](Frame& frame, EntityId entity, Components... components) {
				(system.*func)(frame, entity, components...);
			};

			// Run the system
			static_run_system(tmp::list<Components...>{}, *this, wrapper);
		}

		template <class System, typename Ret, typename ... Components>
		void run_system(System& system, Ret(System::*func)(const Frame& EntityId, Components...)) const
		{
			// Create a wrapper function
			auto wrapper = [&system, func](const Frame& frame, EntityId entity, Components... components) {
				(system.*func)(frame, entity, components...);
			};

			// Run the system
			static_run_system(tmp::list<Components...>{}, *this, wrapper);
		}

	private:

		template <typename SelfT, typename SystemFn, typename ... ComponentTs>
		static void static_run_system(tmp::list<ComponentTs...>, SelfT& self, SystemFn& system)
		{
			// Create wrapper function
			auto invoker = [&system](auto& frame, EntityId entity, const auto* components) {
				invoke_system(tmp::list<ComponentTs...>{}, frame, entity, components, system);
			};

			// Create the array of component types
			const TypeInfo* types[] = { &sge::get_type<typename ComponentTs::ComponentT>()... };

			// Run the system
			self.run_system(invoker, types, sizeof...(ComponentTs));
		}

		template <typename InstanceT, typename SelfT, typename SystemT>
		static void impl_run_system(SelfT& self, SystemT system, const TypeInfo** types, std::size_t numTypes);

		template <typename FrameT, typename CT, typename SystemT, class FrontT, typename ... Rest, typename ... Converted>
		static void invoke_system(
			tmp::list<TComponentInstance<FrontT>, Rest...>,
			FrameT& frame,
			EntityId entity,
			const CT* components,
			SystemT& system,
			const Converted&... converted)
		{
			invoke_system(
				tmp::list<Rest...>{},
				frame,
				entity,
				components + 1,
				system,
				converted...,
				*static_cast<const TComponentInstance<FrontT>*>(components));
		}

		template <typename FrameT, typename SystemT, typename ... Converted>
		static void invoke_system(
			tmp::list<>,
			FrameT& frame,
			EntityId entity,
			const void* /*components*/,
			SystemT& system,
			const Converted&... converted)
		{
			system(frame, entity, converted...);
		}

		void* get_component_object(ComponentId id) const;

		//////////////////
		///   Fields   ///
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
