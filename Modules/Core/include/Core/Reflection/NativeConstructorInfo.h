// NativeConstructorInfo.h
#pragma once

#include <functional>
#include "ConstructorInfo.h"

namespace sge
{
	struct NativeConstructorInfo final : ConstructorInfo
	{
		using Fn = void(void* addr, const ArgAny* args);

		struct Data
		{
			////////////////////////
			///   Constructors   ///
		public:

			Data()
				: constructor(nullptr)
			{
			}

			//////////////////
			///   Fields   ///
		public:

			Fn* constructor;
		};

		////////////////////////
		///   Constructors   ///
	public:

		NativeConstructorInfo(ConstructorInfo::Data baseData, Data data)
			: ConstructorInfo(std::move(baseData)), _data(std::move(data))
		{
		}

		///////////////////
		///   Methods   ///
	public:

		void invoke(void* addr, const ArgAny* args) const override
		{
			_data.constructor(addr, args);
		}

		//////////////////
		///   Fields   ///
	public:

		Data _data;
	};
}
