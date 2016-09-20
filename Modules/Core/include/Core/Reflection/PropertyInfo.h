// PropertyInfo.h
#pragma once

#include <string>
#include <functional>
#include "../Functional/FunctionView.h"

namespace sge
{
	struct Any;

	struct AnyMut;

	struct TypeInfo;

	template <typename F>
	struct FunctionView;

	enum PropertyFlags
	{
		/* This property has no special flags. */
		PF_NONE = 0,

		/* This property should be serialzed. */
		PF_SERIALIZED = (1<<0),

		/* This property is intended exclusively for use by the editor, and should not be exposed to scripting. */
		PF_EDITOR_ONLY = (1<<1)
	};

	struct PropertyInfo
	{
		using GetterOutFn = FunctionView<void(Any)>;

		using MutatorFn = FunctionView<void(AnyMut)>;

		struct Data
		{
			////////////////////////
			///   Constructors   ///
		public:

			Data(const TypeInfo* type, const TypeInfo* contextType, PropertyFlags flags)
				: type{ type }, context_type{ contextType }, flags{ flags }
			{
			}

			//////////////////
			///   Fields   ///
		public:

			PropertyFlags flags;
			const TypeInfo* type;
			const TypeInfo* context_type;
			std::function<void(const void* self, const void* context, GetterOutFn)> getter;
			std::function<void(void* self, void* context, const void* value)> setter;
			std::function<void(void* self, void* context, MutatorFn)> mutate;
		};

		////////////////////////
		///   Constructors   ///
	public:

		PropertyInfo(Data data)
			: _data{ std::move(data) }
		{
		}

		///////////////////
		///   Methods   ///
	public:

		/* The type of this property. */
		const TypeInfo& type() const
		{
			return *_data.type;
		}

		/* The context type required for getting and setting this property. */
		const TypeInfo* context_type()
		{
			return _data.context_type;
		}

		/* Any special flags for this property. */
		PropertyFlags flags() const
		{
			return _data.flags;
		}

		/* Returns whether this property may be read from, but not written to ('set' or 'mutate'). */
		bool is_read_only() const
		{
			return _data.setter == nullptr;
		}

		/* Gets the current value of this property, given instance and context data. */
		void get(const void* self, const void* context, GetterOutFn out) const
		{
			_data.getter(self, context, out);
		}

		/* Sets the current value of this property, given instance and context data. */
		void set(void* self, void* context, const void* value) const
		{
			_data.setter(self, context, value);
		}

		/* Mutates the property. */
		void mutate(void* self, void* context, MutatorFn mutator) const
		{
			_data.mutate(self, context, mutator);
		}

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
