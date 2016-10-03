// NativePropertyInfo.h
#pragma once

#include <functional>
#include "PropertyInfo.h"

namespace sge
{
	struct NativePropertyInfo final : PropertyInfo
	{
		struct Data
		{
			//////////////////
			///   Fields   ///
		public:

			std::function<void(const void* self, const void* context, GetterOutFn out)> getter;
			std::function<void(void* self, void* context, const void* value)> setter;
			std::function<void(void* self, void* context, MutatorFn mutator)> mutate;
		};

		////////////////////////
		///   Constructors   ///
	public:

		NativePropertyInfo(PropertyInfo::Data baseData, Data data)
			: PropertyInfo(std::move(baseData)), _data(std::move(data))
		{
		}

		///////////////////
		///   Methods   ///
	public:

		bool is_read_only() const override
		{
			return _data.setter == nullptr;
		}

		void get(const void* self, const void* context, GetterOutFn out) const override
		{
			_data.getter(self, context, out);
		}

		void set(void* self, void* context, const void* value) const override
		{
			_data.setter(self, context, value);
		}

		void mutate(void* self, void* context, MutatorFn mutator) const override
		{
			_data.mutate(self, context, mutator);
		}

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
