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

	struct SGE_CORE_API InterfaceInfo final
	{
		struct Data
		{
			std::string name;
			std::unordered_map<const TypeInfo*, const void*> implementations;
		};

		////////////////////////
		///   Constructors   ///
	public:

		InterfaceInfo(Data data)
			: _data(std::move(data))
		{
		}

		template <class I>
		InterfaceInfo(InterfaceInfoBuilder<I>&& builder)
			: InterfaceInfo{ std::move(builder).result }
		{
		}

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
