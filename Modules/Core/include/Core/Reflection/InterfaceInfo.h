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

	struct InterfaceInfo final
	{
		struct Data
		{
			////////////////////////
			///   Constructors   ///
		public:

			Data() = default;

			//////////////////
			///   Fields   ///
		public:

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
			: InterfaceInfo(std::move(builder).interface_data)
		{
		}

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
