// InterfaceInfo.h
#pragma once

#include <unordered_map>
#include <string>
#include "../config.h"

namespace sge
{
	struct TypeInfo;

	template <class I>
	struct InterfaceInfoBuilder;

	template <typename T>
	const TypeInfo& get_type();

	struct SGE_CORE_API InterfaceInfo final
	{
		////////////////////////
		///   Constructors   ///
	public:

		InterfaceInfo() = default;

		template <class I>
		InterfaceInfo(InterfaceInfoBuilder<I>&& builder)
			: InterfaceInfo{ std::move(builder).result }
		{
		}

		//////////////////
		///   Fields   ///
	public:

		std::string name;

		std::unordered_map<const TypeInfo*, const void*> implementations;
	};

	template <class I>
	struct InterfaceInfoBuilder final
	{
		////////////////////////
		///   Constructors   ///
	public:

		InterfaceInfoBuilder(std::string name)
		{
			result.name = std::move(name);
		}

		//////////////////
		///   Fields   ///
	public:

		InterfaceInfo result;

		///////////////////
		///   Methods   ///
	public:

		template <typename T>
		InterfaceInfoBuilder&& implemented_for()
		{
			static const I vtable = I::template get_vtable<T>();
			result.implementations.insert(std::make_pair(&get_type<T>(), &vtable));
			return std::move(*this);
		}
	};
}
