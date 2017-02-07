// SystemFrame.h
#pragma once

#include <array>
#include <unordered_set>
#include <Core/Functional/UFunction.h>
#include "ProcessingFrame.h"
#include "TagBuffer.h"
#include "UpdatePipeline.h"
#include "Util/EntityRangePack.h"

namespace sge
{
	struct ProcessingFrame;
	struct SceneData;
	struct Scene;

	enum class ProcessControl
	{
		/**
		 * \brief The process function should stop searching for matches.
		 */
		BREAK,

		/**
		 * \brief The process function should continue searching for matches.
		 * If the processing function return type is void, this is the default value used.
		 */
		CONTINUE
	};

	struct SGE_ENGINE_API SystemFrame
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
		* \param The current value of the user-supplied iterators.
		*/
		using ProcessFn = ProcessControl(ProcessingFrame& pframe, const ComponentInterface* const* components);

        /**
        * \brief Function signature used for processing functions that perform mutation.
        * \param pframe The processing frame for this processing function.
        * \param entity The entity currently being processed.
        * \param components The components attached to this entity currently being processed.
        */
        using ProcessMutFn = ProcessControl(ProcessingFrame& pframe, ComponentInterface* const* components);

		////////////////////////
		///   Constructors   ///
	public:

		SystemFrame(SystemFrame&& move) = default;

	private:

		SystemFrame(UpdatePipeline::SystemToken system_token, const Scene& scene, SceneData& scene_data);
		SystemFrame(const SystemFrame& copy) = delete;
		SystemFrame& operator=(const SystemFrame& copy) = delete;
		SystemFrame& operator=(SystemFrame&& move) = delete;

		///////////////////
		///   Methods   ///
	public:

        /**
         * \brief Returns the token for the system
         */
        UpdatePipeline::SystemToken system_token() const;

        /**
         * \brief Returns the scene this system frame is operating on.
         */
        const Scene& scene() const;

        /**
        * \brief Creates new user entities, and assigns their ids to the given array.
        * The result will be an ordered array of entities.
        */
        void create_entities(EntityId* out_entities, std::size_t num) const;

        /**
        * \brief Destroys the given entities and all of their components. This operates recursively on its children.
        * \param ord_entities The ordered array of entities to destroy.
        */
        void destroy_entities(const EntityId* ord_entities, std::size_t num_entities);

		/**
		* \brief Returns the entity that is the parent of the given entity.
		* \param entity The entity to get the parent of.
		*/
		EntityId get_entity_parent(EntityId entity) const;

        /**
        * \brief Sets the parent of the given entities.
        * \param parent The entity to make the parent. This must be a valid entity.
        * \param ord_children An ordered array of entities to parent to the entity.
        * \param num_children The number of children in the array.
        * NOTE: NULL_ENTITY is not a valid entity, you should use WORLD_ENTITY if you wish to set no parent.
        */
        void set_entities_parent(EntityId parent, const EntityId* ord_children, std::size_t num_children) const;

		/**
		* \brief Retreives the name of the given entity.
		* \param entity The entity to get the name of.
		* \return The name of the entity, or an empty string if the entity does not exist or does not have a name.
		*/
		std::string get_entity_name(EntityId entity) const;

        /**
        * \brief Sets the name of the given entity.
        * \param entity The entity to set the name of. This must be a valid user entity.
        * \param name The name to set. This must be a valid name (an empty string is not a valid name).
        * The name does not need to be unique.
        */
        void set_entity_name(EntityId entity, std::string name) const;

        /**
        * \brief Removes the name from the given entity.
        * \param entity The entity to remove the name from. This has no effect if the given entity does not currently have a name.
        */
        void remove_entity_name(EntityId entity) const;

        /**
        * \brief Requests for new instances of the given type of component to be constructed.
        * \param type The type of component to create. This must be a previously registered component type.
        * \param ord_entities An ordered set of entities for which the component will be created.
        */
        void create_components(const TypeInfo& type, const EntityId* ord_entities, std::size_t num_entities);

        /**
        * \brief Requests the given instances of the given type of component to be destroyed.
        * \param type The type of component to be destroyed.
        * \param ord_entities An ordered array of the entities for the component type to be removed from.
        * \param num_entities The size of the array.
        */
        void destroy_components(const TypeInfo& type, const EntityId* ord_entities, std::size_t num_entities);

		/**
		 * \brief Runs a processing function across all entities that match the given component signature.
		 * \param types The set of required component types to process the entity.
		 * \param num_types The number of given types.
		 * \param process_fn The processing function to call.
		 */
		void process_entities(
			const TypeInfo* const types[],
			std::size_t num_types,
			FunctionView<ProcessFn> process_fn);

        /**
        * \brief Runs the given processing member function across all entities that match the component
        * signature deduced from the processing function argument list.
        * \param outer The object to call the processing member function on.
        * \param process_fn The processing function to call.
        */
        template <class T, typename RetT, typename ... ComponentTs>
        void process_entities(
            T* outer,
            RetT(T::*process_fn)(ProcessingFrame&, ComponentTs...))
        {
            using ComponentList = tmp::list<ComponentTs...>;

            // Create a wrapper function
            auto wrapper = [outer, process_fn](ProcessingFrame& pframe, ComponentTs ... components) -> auto {
                return (outer->*process_fn)(pframe, components...);
            };

            // Run the processing function
            const auto type_array = SystemFrame::component_type_array(ComponentList{});
            auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, wrapper);
            SystemFrame::process_entities(type_array.data(), type_array.size(), adapted);
        }

        /**
        * \brief Runs the given processing function across all entities that match the component
        * signature deduced from the processing function argument list.
        * \param process_fn The processing function to call.
        */
        template <typename ProcessFnT>
        void process_entities(ProcessFnT&& process_fn)
        {
            using FnTraits = stde::function_traits<ProcessFnT>;
            using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 1>;
            using RetT = typename FnTraits::return_type;

            // Run the process function
            const auto type_array = SystemFrame::component_type_array(ComponentList{});
            auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, process_fn);
            SystemFrame::process_entities(type_array.data(), type_array.size(), adapted);
        }

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
        * \brief Runs the given processing function across all entities that match the component
        * signature deduced from the processing function argument list. Allows for mutation.
        * \param process_fn The processing function to call.
        */
        template <typename ProcessFnT>
        void process_entities_mut(ProcessFnT&& process_fn)
        {
            using FnTraits = stde::function_traits<ProcessFnT>;
            using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 1>;
            using RetT = typename FnTraits::return_type;

            // Run the process functions
            const auto type_array = SystemFrame::component_type_array(ComponentList{});
            auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, process_fn);
            SystemFrame::process_entities_mut(type_array.data(), type_array.size(), adapted);
        }

        /**
        * \brief Runs the given processing member function across all entities that match the component
        * signature deduced from the processing function argument list. Allows for mutation.
        * \param outer The object to call the processing member function on.
        * \param process_fn The processing function to call.
        */
        template <class T, typename RetT, class ... ComponentTs>
        void process_entities_mut(
            T* outer,
            RetT(T::*process_fn)(ProcessingFrame&, ComponentTs...))
        {
            using ComponentList = tmp::list<ComponentTs...>;

            // Create a wrapper function
            auto wrapper = [outer, process_fn](ProcessingFrame& pframe, ComponentTs ... components) -> auto {
                return (outer->*process_fn)(pframe, components...);
            };

            // Run the processing function
            const auto type_array = SystemFrame::component_type_array(ComponentList{});
            auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, wrapper);
            SystemFrame::process_entities_mut(type_array.data(), type_array.size(), adapted);
        }

		/**
		 * \brief Processes the union of all entities in the given iterators, as well as those taken from the component containers.
		 * \param user_start_iters An array of start iterators to include in the union function.
		 * \param user_start_iters An array of end iterators to include in the union function.
		 * \param num_user_iters The number of supplied user iterators.
		 * \param types The set of component types to process on the entities.
		 * \param num_types The number of component types given.
		 * \param process_fn The processing function to call.
		 */
        void process_entities(
            const EntityId* const* SGE_RESTRICT user_entity_ranges,
            const std::size_t* SGE_RESTRICT range_lens,
            std::size_t num_required_ranges,
            std::size_t num_inv_required_ranges,
            std::size_t num_ranges,
			const TypeInfo* const types[],
			std::size_t num_types,
			FunctionView<ProcessFn> process_fn);

        /**
        * \brief Processes the union of the given ranges of entities.
        * \param process_fn The processing function to call.
        */
        template <typename ... RangeTs, typename ProcessFnT>
        void process_entities(
            EntityRangePack<RangeTs...> user_ranges,
            ProcessFnT&& process_fn)
        {
            using FnTraits = stde::function_traits<ProcessFnT>;
            using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 1>;
            using RetT = typename FnTraits::return_type;

            // Create the user entity ranges
            const auto user_entity_ranges = user_ranges.get_entity_ranges();
            const auto user_range_lens = user_ranges.get_range_lengths();

            // Run the processing function
            const auto type_array = SystemFrame::component_type_array(ComponentList{});
            auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, process_fn);
            SystemFrame::process_entities(
                user_entity_ranges.data(),
                user_range_lens.data(),
                user_ranges.NUM_RANGES,
                0,
                user_ranges.NUM_RANGES,
                type_array.data(),
                type_array.size(),
                adapted);
        }

        /**
        * \brief Processes the union of the given range of entities.
        * \param outer The object to call the processing member function on.
        * \param process_fn The processing member function to call.
        */
        template <typename ... RangeTs, class T, typename Ret, class ... ComponentTs>
        void process_entities(
            EntityRangePack<RangeTs...> user_ranges,
            T* outer,
            Ret(T::*process_fn)(ProcessingFrame&, ComponentTs...))
        {
            using ComponentList = tmp::list<ComponentTs...>;

            // Create a wrapper function
            auto wrapper = [outer, process_fn](ProcessingFrame& pframe, ComponentTs ... components) -> auto {
                return (outer->*process_fn)(pframe, components...);
            };

            // Create user entity ranges
            const auto user_entity_ranges = user_ranges.get_entity_ranges();
            const auto user_range_lengths = user_ranges.get_range_lengths();

            // Run the processing function
            const auto type_array = SystemFrame::component_type_array(ComponentList{});
            auto adapted = SystemFrame::adapt_process_fn(tmp::type<Ret>{}, ComponentList{}, wrapper);
            SystemFrame::process_entities(
                user_entity_ranges.data(),
                user_range_lengths.data(),
                user_ranges.NUM_RANGES,
                0,
                user_ranges.NUM_RANGSE,
                adapted.first.data(),
                ComponentList::size(),
                adapted.second);
        }

        /**
        * \brief Processes the union of the given entity ranges. Allows for mutation.
        * \param user_start_iters An array of start iterators to include in the union.
        * \param user_end_iters An array of end iterators to process.
        * \param num_user_iters The number of user iterators given.
        * \param types The set of component types to process on the entity.
        * \param num_types The number of component types given.
        * \param process_fn The processing function to call.
        */
        void process_entities_mut(
            const EntityId* const* SGE_RESTRICT user_entity_ranges,
            const std::size_t* SGE_RESTRICT user_range_lengths,
            std::size_t num_required,
            std::size_t num_inv_required,
            std::size_t num_user_ranges,
            const TypeInfo* const types[],
            std::size_t num_types,
            FunctionView<ProcessMutFn> process_fn);

        /**
        * \brief Processes the union of the specified ranges of entities. Allows for mutation.
        * \param user_ranges The user-specified ranges to include in the union.
        * \param outer The object to call the processing member function on.
        * \param process_fn The processing member function to call.
        */
        template <typename ... RangeTs, class T, typename RetT, class ... ComponentTs>
        void process_entities_mut(
            EntityRangePack<RangeTs...> user_ranges,
            T* outer,
            RetT(T::*process_fn)(ProcessingFrame&, ComponentTs...))
        {
            using ComponentList = tmp::list<ComponentTs...>;

            // Create a wrapper function
            auto wrapper = [outer, process_fn](ProcessingFrame& pframe, ComponentTs ... components) -> auto {
                return (outer->*process_fn)(pframe, components...);
            };

            // Create user ranges
            const auto user_entity_ranges = user_ranges.get_entity_ranges();
            const auto user_range_lengths = user_ranges.get_range_lengths();

            // Run the processing function
            const auto type_array = SystemFrame::component_type_array(ComponentList{});
            auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, wrapper);
            SystemFrame::process_entities_mut(
                user_entity_ranges.data(),
                user_range_lengths.data(),
                user_ranges.NUM_RANGES,
                0,
                user_ranges.NUM_RANGES,
                type_array.data(),
                type_array.size(),
                adapted);
        }

        /**
        * \brief Processes the union of the specified ranges of entities. Allows for mutation.
        * \param user_ranges The user-specified ranges to include in the union.
        * \param process_fn The processing function to call.
        */
        template <typename ... RangeTs, typename ProcessFnT>
        void process_entities_mut(
            EntityRangePack<RangeTs...> user_ranges,
            ProcessFnT&& process_fn)
        {
            using FnTraits = stde::function_traits<ProcessFnT>;
            using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 1>;
            using RetT = typename FnTraits::return_type;

            // Create the user entity ranges
            const auto user_entity_ranges = user_ranges.get_entity_ranges();
            const auto user_range_lengths = user_ranges.get_range_lengths();

            // Run the processing function
            const auto type_array = SystemFrame::component_type_array(ComponentList{});
            auto adapted = SystemFrame::adapt_process_fn(tmp::type<RetT>{}, ComponentList{}, process_fn);
            SystemFrame::process_entities_mut(
                user_entity_ranges.data(),
                user_range_lengths.data(),
                user_ranges.NUM_RANGES,
                0,
                user_ranges.NUM_RANGES,
                type_array.data(),
                type_array.size(),
                adapted);
        }

	    /**
         * \brief Appends the specified tag buffer to this system's internal tag buffer set.
         * \param tag_type The type of tag being appended.
         * \param tag_buffer The buffer of tags.
         */
        void append_tags(const TypeInfo& tag_type, TagBuffer tag_buffer);

	private:

		template <typename ... ComponentTs>
		static auto component_type_array(tmp::list<ComponentTs...>)
		{
			return std::array<const TypeInfo*, sizeof...(ComponentTs)>{ &sge::get_type<ComponentTs>()... };
		}

		template <typename ... ComponentTs, typename ProcessFnT>
		static auto adapt_process_fn(tmp::type<ProcessControl>, tmp::list<ComponentTs...>, ProcessFnT& process_fn)
		{
			// Case where return type is 'ProcessControl', return what they return
			return [&process_fn](ProcessingFrame& pframe, auto components) -> ProcessControl {
				return SystemFrame::invoke_process_fn(tmp::list<ComponentTs...>{}, process_fn, pframe, components);
			};
		}

		template <typename ... ComponentTs, typename ProcessFnT>
		static auto adapt_process_fn(tmp::type<void>, tmp::list<ComponentTs...>, ProcessFnT& process_fn)
		{
			// Case where return type is void, just assume they want to unconditionally continue.
			return [&process_fn](ProcessingFrame& pframe, auto components) -> ProcessControl {
				SystemFrame::invoke_process_fn(tmp::list<ComponentTs...>{}, process_fn, pframe, components);
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
			const ComponentIT* components,
			ConvertedCTs&... converted)
		{
			return SystemFrame::invoke_process_fn(
				tmp::list<RestCTs...>{},
				process_fn,
				pframe,
				components + 1,
				converted...,
				*static_cast<std::remove_reference_t<FrontCT>*>(*components));
		}

		template <typename ProcessFnT, typename ComponentIT, class ... ConvertedCTs>
		static auto invoke_process_fn(
			tmp::list<>,
			ProcessFnT& process_fn,
			ProcessingFrame& pframe,
			const ComponentIT* /*components*/,
			ConvertedCTs&... converted)
		{
			return process_fn(pframe, converted...);
		}

		template <typename ProcessFnT>
		void impl_process_entities(
			const EntityId* const* SGE_RESTRICT user_entity_arrays,
            const std::size_t* SGE_RESTRICT user_array_lens,
            std::size_t num_required_arrays,
            std::size_t num_inv_required_arrays,
            std::size_t num_user_arrays,
			const TypeInfo* const types[],
			std::size_t num_types,
			ProcessFnT& process_fn);

		//////////////////
		///   Fields   ///
	private:

        UpdatePipeline::SystemToken _system_token;
        bool _has_tags;
        const Scene* const _scene;
		SceneData* const _scene_data;

        /* All entities that have been destroyed this frame. */
        std::vector<EntityId> _ord_destroyed_entities;

        /* All components that have been destroyed this frame. */
        std::map<const TypeInfo*, std::vector<EntityId>> _ord_destroyed_components;

        /* All components created this frame. */
        std::map<const TypeInfo*, std::vector<EntityId>> _ord_new_components;

        /* All tags generated by processing frames. */
        std::map<const TypeInfo*, std::vector<TagBuffer>> _tags;
	};
}
