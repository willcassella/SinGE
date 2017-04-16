// PropertyInfo.h
#pragma once

#include <utility>
#include "../Functional/FunctionView.h"
#include "Any.h"

namespace sge
{
	struct TypeInfo;

	using PropertyFlags_t = uint32;
	enum PropertyFlags : PropertyFlags_t
	{
		/**
		 * \brief This property has no special flags.
		 */
		PF_NONE = 0,

		/**
		 * \brief This property is intended exclusively for use by the editor, and should not be exposed to scripting.
		 */
		PF_EDITOR_ONLY = (1 << 0),

		/**
		 * \brief This property is not intended to be exposed to the editor.
		 */
		PF_EDITOR_HIDDEN = (1 << 1),

		/**
		 * \brief By default, the value of this property is not visible in the editor unless expanded.
		 */
		PF_EDITOR_DEFAULT_COLLAPSED = (1 << 2)
	};

	struct SGE_CORE_API PropertyInfo
	{
		using GetterOutFn = FunctionView<void(Any<> value)>;
		using MutatorFn = FunctionView<void(AnyMut<> value)>;

		struct Data
		{
			////////////////////////
			///   Constructors   ///
		public:

			Data()
				: flags(PF_NONE),
				index(0),
				type(nullptr)
			{
			}

			//////////////////
			///   Fields   ///
		public:

			PropertyFlags_t flags;
			uint32 index;
			std::string category;
			std::string description;
			const TypeInfo* type;
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
		 * \brief The type of this property.
		 */
		const TypeInfo& type() const
		{
			return *_data.type;
		}

		/**
		 * \brief Any special flags for this property.
		 */
		PropertyFlags_t flags() const
		{
			return _data.flags;
		}

		/**
		 * \brief Returns the property registration index (used to order properties).
		 */
		uint32 index() const
		{
			return _data.index;
		}

		/**
		 * \brief Returns the categorry for this property.
		 */
		const std::string& category() const
		{
			return _data.category;
		}

		/**
		 * \brief Returns the description for this property.
		 */
		const std::string& description() const
		{
			return _data.description;
		}

		/**
		 * \brief Whether this property may be read from, but not written to ('set' or 'mutate').
		 */
		virtual bool is_read_only() const = 0;

		/**
		 * \brief Acesses the value of this property.
		 * \param self The object to access the property on.
		 * \param out A function to call with the value and type of the property.
		 */
		virtual void get(const void* self, GetterOutFn out) const = 0;

		/**
		 * \brief Sets the value of this property.
		 * \param self The object to set the property on.
		 * \param value The new value of the property.
		 */
		virtual void set(void* self, const void* value) const = 0;

		/**
		 * \brief Mutates the value of this property.
		 * \param self The object to mutate the property on.
		 * \param mutator A function to call that will mutate the value of the property.
		 */
		virtual void mutate(void* self, MutatorFn mutator) const = 0;

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
