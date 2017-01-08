// SystemFrame.h
#pragma once

#include <array>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include "Component.h"

namespace sge
{
	struct ProcessingFrame;
	struct SceneData;
	struct Scene;

	enum class ProcessControl
	{
		/**
		 * \brief The process function should stop searching for matches. This has no effect for 'process_single' functions.
		 */
		BREAK,

		/**
		 * \brief The process function should continue searching for matches. This has no effect for 'process_single' functions.
		 */
		CONTINUE
	};

	class SGE_ENGINE_API SystemFrame
	{
	public:

		SGE_REFLECTED_TYPE;

		/* Only 'Scene' objects may construct SystemFrames. */
		friend Scene;

		/**
		* \brief Function signature used for processing functions that do not perform mutation.
		* \param pframe The processing frame for this processing function.
		* \param entity The entity currently being processed.
		* \param components The components attached to this entity currently being processed.
		*/
		using ProcessFn = ProcessControl(ProcessingFrame& pframe, EntityId entity, const ComponentInterface* const components[]);

		////////////////////////
		///   Constructors   ///
	protected:

		SystemFrame(const Scene& scene, const SceneData& scene_data);
		SystemFrame(const SystemFrame& copy) = delete;
		SystemFrame& operator=(const SystemFrame& copy) = delete;
		SystemFrame(SystemFrame&& move) = delete;
		SystemFrame& operator=(SystemFrame&& move) = delete;

		///////////////////
		///   Methods   ///
	public:

		const Scene& get_scene() const;

		/**
		 * \brief Synchronizes this SystemFrame. This function blocks the calling thread until
		 * all other system functions have called 'sync' or returned. Then it flushes all pending changes and returns.
		 */
		void sync();

		/**
		* \brief Returns the entity that is the parent of the given entity.
		* \param entity The entity to get the parent of. This must be a valid user entity.
		* \return The parent of the given entity, or NULL_ENTITY if the given entity is not a valid user entity.
		*/
		EntityId get_entity_parent(EntityId entity) const;

		/**
		* \brief Retreives the name of the given entity.
		* \param entity The entity to get the name of.
		* \return The name of the entity, or an empty string if the entity does not exist or does not have a name.
		*/
		std::string get_entity_name(EntityId entity) const;

		/**
		 * \brief Runs a processing function across all entities that match the given component signature.
		 * \param types The set of required component types to process the entity.
		 * \param num_types The number of given types.
		 * \param process_fn The processing function to call.
		 */
		void process_entities(
			const TypeInfo* const types[],
			std::size_t num_types,
			FunctionView<ProcessFn> process_fn) const;

		/**
		 * \brief Processes a single entity, if it has the given set of component types.
		 * \param entity The entity to process.
		 * \param types The set of component types to process on the entity.
		 * \param num_types The number of component types given.
		 * \param process_fn The processing function to call.
		 */
		void process_single(
			EntityId entity,
			const TypeInfo* const types[],
			std::size_t num_types,
			FunctionView<ProcessFn> process_fn) const;

		/**
		 * \brief Runs the given processing function across all entities that match the component
		 * signature deduced from the processing function argument list.
		 * \param process_fn The processing function to call.
		 */
		template <typename ProcessFnT>
		void process_entities(ProcessFnT&& process_fn) const
		{
			using FnTraits = stde::function_traits<ProcessFnT>;
			using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 2>;
			using RetT = typename FnTraits::return_type;

			// Run the process function
			auto type_array = SystemFrame::component_type_array(ComponentList{});
			auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, process_fn);
			SystemFrame::process_entities(type_array.data(), type_array.size(), adapted);
		}

		/**
		 * \brief Processes a single entity, if it has the set of components
		 * deduced from the processing function argument list.
		 * \param entity The entity to process.
		 * \param process_fn The processing function to call.
		 */
		template <typename ProcessFnT>
		void process_single(EntityId entity, ProcessFnT&& process_fn)
		{
			using FnTraits = stde::function_traits<ProcessFnT>;
			using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 2>;
			using RetT = typename FnTraits::return_type;

			// Run the processing function
			auto type_array = SystemFrame::component_type_array(ComponentList{});
			auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, process_fn);
			SystemFrame::process_single(entity, type_array.data(), type_array.size(), adapted);
		}

		/**
		 * \brief Runs the given processing member function across all entities that match the component
		 * signature deduced from the processing function argument list.
		 * \param outer The object to call the processing member function on.
		 * \param process_fn The processing function to call.
		 */
		template <class T, typename RetT, typename ... ComponentTs>
		void process_entities(T& outer, RetT(T::*process_fn)(ProcessingFrame&, EntityId, ComponentTs...)) const
		{
			using ComponentList = tmp::list<ComponentTs...>;

			// Create a wrapper function
			auto wrapper = [&outer, process_fn](ProcessingFrame& pframe, EntityId entity, ComponentTs ... components) -> auto {
				return (outer.*process_fn)(pframe, entity, components...);
			};

			// Run the processing function
			auto type_array = SystemFrame::component_type_array(ComponentList{});
			auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, wrapper);
			SystemFrame::process_entities(type_array.data(), type_array.size(), adapted);
		}

		/**
		 * \brief Processes a single entity, if it has the set of components
		 * deduced from the processing member function argument list.
		 * \param entity The entity to process.
		 * \param outer The object to call the processing member function on.
		 * \param process_fn The processing member function to call.
		 */
		template <class T, typename Ret, class ... ComponentTs>
		void process_single(EntityId entity, T& outer, Ret(T::*process_fn)(ProcessingFrame&, EntityId, ComponentTs...)) const
		{
			using ComponentList = tmp::list<ComponentTs...>;

			// Create a wrapper function
			auto wrapper = [&outer, process_fn](ProcessingFrame& pframe, EntityId entity, ComponentTs ... components) -> auto {
				return (outer.*process_fn)(pframe, entity, components...);
			};

			// Run the processing function
			auto adapted = SystemFrame::adapt_process_fn(ComponentList{}, wrapper);
			SystemFrame::process_single(entity, adapted.first.data(), ComponentList::size(), adapted.second);
		}

	protected:

		template <typename ... ComponentTs>
		static auto component_type_array(tmp::list<ComponentTs...>)
		{
			return std::array<const TypeInfo*, sizeof...(ComponentTs)>{ &sge::get_type<ComponentTs>()... };
		}

		template <typename ... ComponentTs, typename ProcessFnT>
		static auto adapt_process_fn(tmp::type<ProcessControl>, tmp::list<ComponentTs...>, ProcessFnT& process_fn)
		{
			// Case where return type is 'ProcessControl', return what they return
			return [&process_fn](ProcessingFrame& pframe, EntityId entity, auto components) -> ProcessControl {
				return SystemFrame::invoke_process_fn(tmp::list<ComponentTs...>{}, process_fn, pframe, entity, components);
			};
		}

		template <typename ... ComponentTs, typename ProcessFnT>
		static auto adapt_process_fn(tmp::type<void>, tmp::list<ComponentTs...>, ProcessFnT& process_fn)
		{
			// Case where return type is void, just assume they want to unconditionally continue.
			return [&process_fn](ProcessingFrame& pframe, EntityId entity, auto components) -> ProcessControl {
				SystemFrame::invoke_process_fn(tmp::list<ComponentTs...>{}, process_fn, pframe, entity, components);
				return ProcessControl::CONTINUE;
			};
		}

		template <typename RetT, typename ComponentTs, typename ProcessFnT>
		static void adapt_process_fn(tmp::type<RetT>, ComponentTs, ProcessFnT&)
		{
			static_assert(tmp::failure<RetT>::value, "The return type for this process function is not supported.");
		}

		template <typename ProcessFnT, typename ComponentIT, class FrontCT, class ... RestCTs, class ... ConvertedCTs>
		static auto invoke_process_fn(
			tmp::list<FrontCT, RestCTs...>,
			ProcessFnT& process_fn,
			ProcessingFrame& pframe,
			EntityId entity,
			const ComponentIT* components,
			ConvertedCTs&... converted)
		{
			return SystemFrame::invoke_process_fn(
				tmp::list<RestCTs...>{},
				process_fn,
				pframe,
				entity,
				components + 1,
				converted...,
				*static_cast<std::remove_reference_t<FrontCT>*>(*components));
		}

		template <typename ProcessFnT, typename ComponentIT, class ... ConvertedCTs>
		static auto invoke_process_fn(
			tmp::list<>,
			ProcessFnT& process_fn,
			ProcessingFrame& pframe,
			EntityId entity,
			const ComponentIT* /*components*/,
			ConvertedCTs&... converted)
		{
			return process_fn(pframe, entity, converted...);
		}

		template <typename SelfT, typename SceneDataT, typename ProcessFnT>
		static void impl_process_entities(
			SelfT& self,
			SceneDataT& scene_data,
			const TypeInfo* const types[],
			std::size_t num_types,
			ProcessFnT& process_fn);

		template <typename SelfT, typename SceneDataT, typename ProcessFnT>
		static void impl_process_single(
			SelfT& self,
			SceneDataT& scene_data,
			EntityId entity,
			const TypeInfo* const types[],
			std::size_t num_types,
			ProcessFnT& process_fn);

		virtual void flush_changes();

		//////////////////
		///   Fields   ///
	private:

		const Scene* _scene;
		const SceneData* _scene_data;
	};

	class SGE_ENGINE_API SystemFrameMut final : public SystemFrame
	{
	public:

		SGE_REFLECTED_TYPE;

		/* Only 'Scene' objects may construct SystemFrameMuts. */
		friend Scene;

		/**
		 * \brief Function called when initializing a component.
		 * \param entity
		 * \param component
		 */
		using ComponentInitFn = void(EntityId entity, ComponentInterface* component);

		/**
		* \brief Function signature used for processing functions that perform mutation.
		* \param pframe The processing frame for this processing function.
		* \param entity The entity currently being processed.
		* \param components The components attached to this entity currently being processed.
		*/
		using ProcessMutFn = ProcessControl(ProcessingFrame& pframe, EntityId entity, ComponentInterface* const components[]);

		////////////////////////
		///   Constructors   ///
	private:

		SystemFrameMut(const Scene& scene, SceneData& scene_data);
		SystemFrameMut(const SystemFrameMut& copy) = delete;
		SystemFrameMut& operator=(const SystemFrameMut& copy) = delete;
		SystemFrameMut(SystemFrameMut&& move) = delete;
		SystemFrameMut& operator=(SystemFrameMut&& move) = delete;

		///////////////////
		///   Methods   ///
	public:

		/**
		* \brief Creates a new user entity, and returns its Id.
		*/
		EntityId create_entity() const;

		/**
		 * \brief Destroys the given entity and all of its components. This operates recursively on its children.
		 * \param entity The entity to destroy. This must be a valid user entity.
		 */
		void destroy_entity(EntityId entity);

		/**
		* \brief Sets the parent of the given entity.
		* \param entity The entity to sett the parent of. This must be a valid user entity.
		* \param parent The parent entity to set. This must be a valid entity.
		* NULL_ENTITY is not a valid entity, you should use WORLD_ENTITY if you wish to set no parent.
		*/
		void set_entity_parent(EntityId entity, EntityId parent) const;

		/**
		* \brief Sets the name of the given entity.
		* \param entity The entity to set the name of. This must be a valid user entity.
		* \param name The name to set. This must be a valid name (an empty string is not a valid name).
		* The name does not need to be unique.
		*/
		void set_entity_name(EntityId entity, std::string name) const;

		/**
		* \brief Removes the name from the given entity.
		* \param entity The entity to remove the name from. This has no effect if the given entity is not a valid user entity,
		* or the entity does not currently have a name.
		*/
		void remove_entity_name(EntityId entity) const;

		/**
		 * \brief Constructs a new component of the given type on the given entity, and initializes it with the given function.
		 * \param entity The entity to create the component on. This must be a valid user entity.
		 * \param type The type of component to create. This must be a previously registered component type.
		 * \param init_fn The function to initialize the component with.
		 * \return The id of the component. The component will not yet have been constructed at this point.
		 * If the entity is not a valid user entity or the type is not a registered component type, the returned id will be null.
		 */
		ComponentId new_component(EntityId entity, const TypeInfo& type, std::function<ComponentInitFn> init_fn);

		/**
		 * \brief Constructs a new component of the given type on the given entity, and initializes it with the given function.
		 * \tparam C The type of component to create. This must be a previously registered component type.
		 * \param entity The entity to create the component on. This must be a valid user entity.
		 * \param init_fn The function to initialize the component with.
		 * \return The id of the component. The component will not yet have been constructed at this point.
		 */
		template <class C, typename InitFn>
		TComponentId<C> new_component(EntityId entity, InitFn&& init_fn)
		{
			// Generate a wrapper function
			auto wrapper = [init_fn](EntityId entity, ComponentInterface* component) -> void {
				init_fn(entity, *static_cast<C*>(component));
			};

			// Insert it into the table
			this->new_component(entity, sge::get_type<C>(), std::move(wrapper));

			// Generate an Id
			return TComponentId<C>{ entity };
		}

		/**
		 * \brief Destroys the given component.
		 * \param component The id component to remove.
		 */
		void destroy_component(ComponentId component);

		/**
		 * \brief Destroys the given component.
		 * \param entity The entity that owns the component to be destroyed.
		 * \param type The type of component to be destroyed.
		 */
		void destroy_component(EntityId entity, const TypeInfo& type);

		/**
		* \brief Runs a processing function across all entities that match the given component signature, allowing for mutation.
		* \param types The set of required component types to process the entity.
		* \param num_types The number of given types.
		* \param process_fn The processing function to call.
		*/
		void process_entities_mut(
			const TypeInfo* const types[],
			std::size_t num_types,
			FunctionView<ProcessMutFn> process_fn);

		/**
		* \brief Processes a single entity, if it has the given set of component types. Allows for mutation.
		* \param entity The entity to process.
		* \param types The set of component types to process on the entity.
		* \param num_types The number of component types given.
		* \param process_fn The processing function to call.
		*/
		void process_single_mut(
			EntityId entity,
			const TypeInfo* const types[],
			std::size_t num_types,
			FunctionView<ProcessMutFn> process_fn);

		/**
		* \brief Runs the given processing function across all entities that match the component
		* signature deduced from the processing function argument list. Allows for mutation.
		* \param process_fn The processing function to call.
		*/
		template <typename ProcessFnT>
		void process_entities_mut(ProcessFnT&& process_fn)
		{
			using FnTraits = stde::function_traits<ProcessFnT>;
			using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 2>;
			using RetT = typename FnTraits::return_type;

			// Run the process functions
			auto type_array = SystemFrame::component_type_array(ComponentList{});
			auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, process_fn);
			SystemFrameMut::process_entities_mut(type_array.data(), type_array.size(), adapted);
		}

		/**
		* \brief Processes a single entity, if it has the set of components
		* deduced from the processing function argument list. Allows for mutation.
		* \param entity The entity to process.
		* \param process_fn The processing function to call.
		*/
		template <typename ProcessFnT>
		void process_single_mut(EntityId entity, ProcessFnT&& process_fn)
		{
			using FnTraits = stde::function_traits<ProcessFnT>;
			using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 2>;
			using RetT = typename FnTraits::return_type;

			// Run the processing function
			auto type_array = SystemFrame::component_type_array(ComponentList{});
			auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, process_fn);
			SystemFrameMut::process_single_mut(entity, type_array.data(), type_array.size(), adapted);
		}

		/**
		* \brief Runs the given processing member function across all entities that match the component
		* signature deduced from the processing function argument list. Allows for mutation.
		* \param outer The object to call the processing member function on.
		* \param process_fn The processing function to call.
		*/
		template <class T, typename RetT, class ... ComponentTs>
		void process_entities_mut(T& outer, RetT(T::*process_fn)(ProcessingFrame&, EntityId, ComponentTs...))
		{
			using ComponentList = tmp::list<ComponentTs...>;

			// Create a wrapper function
			auto wrapper = [&outer, process_fn](ProcessingFrame& pframe, EntityId entity, ComponentTs ... components) -> auto {
				return (outer.*process_fn)(pframe, entity, components...);
			};

			// Run the processing function
			auto type_array = SystemFrame::component_type_array(ComponentList{});
			auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, wrapper);
			SystemFrameMut::process_entities_mut(type_array.data(), type_array.size(), adapted);
		}

		/**
		* \brief Processes a single entity, if it has the set of components
		* deduced from the processing member function argument list. Allows for mutation.
		* \param entity The entity to process.
		* \param outer The object to call the processing member function on.
		* \param process_fn The processing member function to call.
		*/
		template <class T, typename RetT, class ... ComponentTs>
		void process_single_mut(EntityId entity, T& outer, RetT(T::*process_fn)(ProcessingFrame&, EntityId, ComponentTs...))
		{
			using ComponentList = tmp::list<ComponentTs...>;

			// Create a wrapper function
			auto wrapper = [&outer, process_fn](ProcessingFrame& pframe, EntityId entity, ComponentTs ... components) -> auto {
				return (outer.*process_fn)(pframe, entity, components...);
			};

			// Run the processing function
			auto type_array = SystemFrame::component_type_array(ComponentList{});
			auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, wrapper);
			SystemFrameMut::process_single_mut(entity, type_array.data(), type_array.size(), adapted);
		}

	private:

		void flush_changes() override;

		//////////////////
		///   Fields   ///
	private:

		/* Scene Data */
		SceneData* _scene_data_mut;

		/* All components that have been destroyed this frame. */
		std::unordered_set<ComponentId> _destroyed_components;

		/* All entities that have been destroyed this frame. */
		std::unordered_set<EntityId> _destroyed_entities;

		/* All components created this frame. */
		std::unordered_map<ComponentId, std::function<ComponentInitFn>> _new_components;
	};
}
