// Scene.h
#pragma once

#include <array>
#include <memory>
#include <unordered_map>
#include "ProcessingFrame.h"

namespace sge
{
	struct TypeDB;

	struct SGE_ENGINE_API Scene
	{
		SGE_REFLECTED_TYPE;

		using ProcessMutFn = void(ProcessingFrameMut& pframe, EntityId entity, ComponentInterface* const components[]);
		using ProcessFn = void(ProcessingFrame& pframe, EntityId entity, const ComponentInterface* const components[]);
		using ComponentTypeEnumeratorFn = void(const TypeInfo& type);

		////////////////////////
		///   Constructors   ///
	public:

		Scene(TypeDB& typedb);
		Scene(const Scene& copy) = delete;
		Scene& operator=(const Scene& copy) = delete;
		Scene(Scene&& move) = default;
		Scene& operator=(Scene&& move) = default;
		~Scene();

		///////////////////
		///   Methods   ///
	public:

		void to_archive(ArchiveWriter& writer) const;

		void from_archive(ArchiveReader& reader);

		void register_component_type(const TypeInfo& type, std::unique_ptr<ComponentContainer> container);

		TypeDB& get_type_db() const;

		const TypeInfo* get_component_type(const char* typeName) const;

		void enumerate_component_types(FunctionView<ComponentTypeEnumeratorFn> enumerator) const;

		EntityId new_entity();

		EntityId get_entity_parent(EntityId entity) const;

		void set_entity_parent(EntityId entity, EntityId parent);

		std::string get_entity_name(EntityId entity) const;

		void set_entity_name(EntityId entity, std::string name);

		ComponentId new_component(EntityId entity, const TypeInfo& type);

		void process_entities_mut(const TypeInfo* const types[], std::size_t numTypes, FunctionView<ProcessMutFn> processFn);

		void process_entities(const TypeInfo* const types[], std::size_t numTypes, FunctionView<ProcessFn> processFn) const;

		void process_single_mut(EntityId entity, const TypeInfo* const types[], std::size_t numTypes, FunctionView<ProcessMutFn> processFn);

		void process_single(EntityId entity, const TypeInfo* const types[], std::size_t numTypes, FunctionView<ProcessFn> processFn) const;

		template <typename ProcessFnT>
		void process_entities_mut(ProcessFnT&& processFn)
		{
			using FnTraits = stde::function_traits<ProcessFnT>;
			using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 2>;

			// Run the process functions
			auto adapted = adapt_process_fn(ComponentList{}, processFn);
			process_entities_mut(adapted.first.data(), ComponentList::size(), adapted.second);
		}

		template <typename ProcessFnT>
		void process_entities(ProcessFnT&& processFn) const
		{
			using FnTraits = stde::function_traits<ProcessFnT>;
			using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 2>;

			// Run the process function
			auto adapted = adapt_process_fn(ComponentList{}, processFn);
			process_entities(adapted.first.data(), ComponentList::size(), adapted.second);
		}

		template <typename ProcessFnT>
		void process_single_mut(EntityId entity, ProcessFnT&& processFn)
		{
			using FnTraits = stde::function_traits<ProcessFnT>;
			using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 2>;

			// Run the processing function
			auto adapted = adapt_process_fn(ComponentList{}, processFn);
			process_single_mut(entity, adapted.first.data(), ComponentList::size(), adapted.second);
		}

		template <typename ProcessFnT>
		void process_single(EntityId entity, ProcessFnT&& processFn)
		{
			using FnTraits = stde::function_traits<ProcessFnT>;
			using ComponentList = tmp::cdr_n<typename FnTraits::arg_types, 2>;

			// Run the processing function
			auto adapted = adapt_process_fn(ComponentList{}, processFn);
			process_single(entity, adapted.first.data(), ComponentList::size(), adapted.second);
		}

		template <class T, typename Ret, class ... ComponentTs>
		void process_entities_mut(T& outer, Ret(T::*processFn)(ProcessingFrameMut&, EntityId, ComponentTs...))
		{
			using ComponentList = tmp::list<ComponentTs...>;

			// Create a wrapper function
			auto wrapper = [&outer, processFn](ProcessingFrameMut& pframe, EntityId entity, ComponentTs ... components) {
				(outer.*processFn)(pframe, entity, components...);
			};

			// Run the processing function
			auto adapted = adapt_process_fn(ComponentList{}, wrapper);
			process_entities_mut(adapted.first.data(), ComponentList::size(), adapted.second);
		}

		template <class T, typename Ret, typename ... ComponentTs>
		void process_entities(T& outer, Ret(T::*processFn)(ProcessingFrame&, EntityId, ComponentTs...)) const
		{
			using ComponentList = tmp::list<ComponentTs...>;

			// Create a wrapper function
			auto wrapper = [&outer, processFn](ProcessingFrame& pframe, EntityId entity, ComponentTs ... components) {
				(outer.*processFn)(pframe, entity, components...);
			};

			// Run the processing function
			auto adapted = adapt_process_fn(ComponentList{}, wrapper);
			process_entities(adapted.first.data(), ComponentList::size(), adapted.second);
		}

		template <class T, typename Ret, class ... ComponentTs>
		void process_single_mut(EntityId entity, T& outer, Ret(T::*processFn)(ProcessingFrameMut&, EntityId, ComponentTs...))
		{
			using ComponentList = tmp::list<ComponentTs...>;

			// Create a wrapper function
			auto wrapper = [&outer, processFn](ProcessingFrameMut& pframe, EntityId entity, ComponentTs ... components) -> void {
				(outer.*processFn)(pframe, entity, components...);
			};

			// Run the processing function
			auto adapted = adapt_process_fn(ComponentList{}, wrapper);
			process_single_mut(entity, adapted.first.data(), ComponentList::size(), adapted.second);
		}

		template <class T, typename Ret, class ... ComponentTs>
		void process_single(EntityId entity, T& outer, Ret(T::*processFn)(ProcessingFrame&, EntityId, ComponentTs...)) const
		{
			using ComponentList = tmp::list<ComponentTs...>;

			// Create a wrapper function
			auto wrapper = [&outer, processFn](ProcessingFrame& pframe, EntityId entity, ComponentTs ... components) -> void {
				(outer.*processFn)(pframe, entity, components...);
			};

			// Run the processing function
			auto adapted = adapt_process_fn(ComponentList{}, wrapper);
			process_single(entity, adapted.first.data(), ComponentList::size(), adapted.second);
		}

	private:

		template <typename ... ComponentTs, typename ProcessFnT>
		static auto adapt_process_fn(tmp::list<ComponentTs...>, ProcessFnT& processFn)
		{
			// Create the array of component types
			std::array<const TypeInfo*, sizeof...(ComponentTs)> types = { &sge::get_type<ComponentTs>()... };

			// Create wrapper function
			auto invoker = [&processFn](auto& pframe, EntityId entity, auto components) {
				invoke_process_fn(tmp::list<ComponentTs...>{}, processFn, pframe, entity, components);
			};

			return std::make_pair(types, invoker);
		}

		template <typename ProcessFnT, typename PFrameT, typename ComponentIT, class FrontCT, class ... RestCTs, class ... ConvertedCTs>
		static void invoke_process_fn(
			tmp::list<FrontCT, RestCTs...>,
			ProcessFnT& processFn,
			PFrameT& pframe,
			EntityId entity,
			const ComponentIT* components,
			ConvertedCTs&... converted)
		{
			invoke_process_fn(
				tmp::list<RestCTs...>{},
				processFn,
				pframe,
				entity,
				components + 1,
				converted...,
				*static_cast<std::remove_reference_t<FrontCT>*>(*components));
		}

		template <typename ProcessFnT, typename PFrameT, typename ComponentIT, class ... ConvertedCTs>
		static void invoke_process_fn(
			tmp::list<>,
			ProcessFnT& processFn,
			PFrameT& pframe,
			EntityId entity,
			const ComponentIT* /*components*/,
			ConvertedCTs&... converted)
		{
			processFn(pframe, entity, converted...);
		}

		template <typename PFrameT, typename SelfT, typename ProcessFnT>
		static void impl_process_entities(SelfT& self, const TypeInfo* const types[], std::size_t numTypes, ProcessFnT& processFn);

		template <typename PFrameT, typename SelfT, typename ProcessFnT>
		static void impl_process_single(SelfT& self, EntityId entity, const TypeInfo* const types[], std::size_t numTypes, ProcessFnT& processFn);

		//////////////////
		///   Fields   ///
	private:

		EntityId _next_entity_id;
		float _current_time;

		/* Entity Data */
		std::unordered_map<EntityId, EntityId> _entity_parents;
		std::unordered_map<EntityId, std::string> _entity_names;

		/* Component Data */
		std::unordered_map<const TypeInfo*, std::unique_ptr<ComponentContainer>> _components;
		TypeDB* _type_db;
	};
}
