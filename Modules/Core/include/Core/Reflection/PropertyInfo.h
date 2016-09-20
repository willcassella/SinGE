// PropertyInfo.h
#pragma once

#include <string>
#include <functional>

namespace sge
{
	struct Any;

	struct AnyMut;

	struct TypeInfo;

	template <typename F>
	struct FunctionView;

	template <typename T>
	const TypeInfo& get_type();

	enum PropertyFlags
	{
		/* This property has no special flags. */
		PF_NONE = 0,

		/* This property should be serialzed. */
		PF_SERIALIZED = (1<<0),

		/* This property is intended exclusively for this editor, and should not be exposed to scripting. */
		PF_EDITOR_ONLY = (1<<1)
	};

	struct PropertyInfo
	{
		using GetterOutFn = FunctionView<void(Any)>;

		using MutatorFn = FunctionView<void(AnyMut)>;

		////////////////////////
		///   Constructors   ///
	public:

		template <typename PropT>
		static PropertyInfo create(std::string name, PropertyFlags flags, const TypeInfo* contextType)
		{
			PropertyInfo result;
			result.name = std::move(name);
			result.type = &get_type<PropT>();
			result.context_type = contextType;
			result.flags = flags;
			result.getter = nullptr;
			result.setter = nullptr;
			result.mutate = nullptr;

			return result;
		}

		//////////////////
		///   Fields   ///
	public:

		/* The name of this property. */
		std::string name;

		/* The type of this property. */
		const TypeInfo* type;

		/* The context type required for getting and setting this property. */
		const TypeInfo* context_type;

		/* Any special flags for this property. */
		PropertyFlags flags;

		/* Gets the current value of this property, given instance and context data. */
		std::function<void(const void* self, const void* context, GetterOutFn)> getter;

		/* Sets the current value of this property, given instance and context data. */
		std::function<void(void* self, void* context, const void* value)> setter;

		/* Mutates the property. */
		std::function<void(void* self, void* context, MutatorFn)> mutate;
	};
}
