// PropertyInfo.h
#pragma once

#include <string>
#include <functional>
#include "../Functional/FunctionView.h"
#include "Any.h"

namespace sge
{
	struct TypeInfo;

	template <typename T>
	const TypeInfo& get_type();

	enum PropertyFlags
	{
		PF_NONE = 0,
		PF_TRANSIENT = 1,
		PF_EDITOR_ONLY = 2
	};

	struct PropertyInfo
	{
		using GetterOut = FunctionView<void(Any)>;

		using MutGetterOut = FunctionView<void(AnyMut)>;

		////////////////////////
		///   Constructors   ///
	public:

		template <typename PropT>
		static PropertyInfo create(std::string name, PropertyFlags flags)
		{
			PropertyInfo result;
			result.name = std::move(name);
			result.type = &get_type<PropT>();
			result.flags = flags;
			result.getter = nullptr;
			result.setter = nullptr;

			return result;
		}

		//////////////////
		///   Fields   ///
	public:

		/* The name of this property. */
		std::string name;
			
		/* The type of this property. */
		const TypeInfo* type;

		/* Any special flags for this property. */
		PropertyFlags flags;

		/* Gets the current value of this property, given instance and context data. */
		std::function<void(const void* self, const void* context, GetterOut)> getter;

		/* Sets the current value of this property, given instance and context data. */
		std::function<void(void* self, const void* context, const void* value)> setter;
	};
}
