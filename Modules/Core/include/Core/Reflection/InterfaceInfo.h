// InterfaceInfo.h
#pragma once

#include <string>
#include "../config.h"

namespace singe
{
	template <typename T>
	struct InterfaceInfoBuilder;

	struct CORE_API InterfaceInfo final
	{
		////////////////////////
		///   Constructors   ///
	public:

		InterfaceInfo();

		template <typename T>
		InterfaceInfo(InterfaceInfoBuilder<T>&& builder)
			: InterfaceInfo{ std::move(builder.result) }
		{
		}

		//////////////////
		///   Fields   ///
	public:

		std::string name;
	};

	template <typename T>
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
	};
}
