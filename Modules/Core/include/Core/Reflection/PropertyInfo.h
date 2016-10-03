// PropertyInfo.h
#pragma once

#include <utility>
#include "../Functional/FunctionView.h"

namespace sge
{
	struct Any;
	struct AnyMut;
	struct TypeInfo;

	enum PropertyFlags
	{
		/**
		 * \breif This property has no special flags.
		 */
		PF_NONE = 0,

		/**
		 * \breif This property should be serialzed.
		 */
		PF_SERIALIZED = (1<<0),

		/**
		 * \breif This property is intended exclusively for use by the editor, and should not be exposed to scripting.
		 */
		PF_EDITOR_ONLY = (1<<1)
	};

	struct PropertyInfo
	{
		using GetterOutFn = FunctionView<void(Any value)>;
		using MutatorFn = FunctionView<void(AnyMut value)>;

		struct Data
		{
			////////////////////////
			///   Constructors   ///
		public:

			Data()
				: type(nullptr), context_type(nullptr), flags(PF_NONE)
			{
			}

			//////////////////
			///   Fields   ///
		public:

			const TypeInfo* type;
			const TypeInfo* context_type;
			PropertyFlags flags;
		};

		////////////////////////
		///   Constructors   ///
	public:

		PropertyInfo(Data data)
			: _data(std::move(data))
		{
		}

		///////////////////
		///   Methods   ///
	public:

		/**
		 * \breif The type of this property.
		 */
		const TypeInfo& type() const
		{
			return *_data.type;
		}

		/**
		 * \breif The context type required for getting and setting this property.
		 */
		const TypeInfo* context_type() const
		{
			return _data.context_type;
		}

		/**
		 * \breif Any special flags for this property.
		 */
		PropertyFlags flags() const
		{
			return _data.flags;
		}

		/**
		 * \breif Whether this property may be read from, but not written to ('set' or 'mutate').
		 */
		virtual bool is_read_only() const = 0;

		/**
		 * \brief Acesses the value of this property.
		 * \param self The object to access the property on.
		 * \param context The context object for accessing the property.
		 * \param out A function to call with the value and type of the property.
		 */
		virtual void get(const void* self, const void* context, GetterOutFn out) const = 0;

		/**
		 * \brief Sets the value of this property.
		 * \param self The object to set the property on.
		 * \param context The context object for accessing the property.
		 * \param value The new value of the property.
		 */
		virtual void set(void* self, void* context, const void* value) const = 0;

		/**
		 * \brief Mutates the value of this property.
		 * \param self THe object to mutate the property on.
		 * \param context The context object for accessing the property.
		 * \param mutator A function to call that will mutate the value of the property.
		 */
		virtual void mutate(void* self, void* context, MutatorFn mutator) const = 0;

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
