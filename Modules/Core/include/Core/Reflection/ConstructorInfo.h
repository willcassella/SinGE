// ConstructorInfo.h
#pragma once

#include <vector>

namespace sge
{
	struct TypeInfo;
	struct ArgAny;

	struct SGE_CORE_API ConstructorInfo
	{
		struct Data
		{
			//////////////////
			///   Fields   ///
		public:

			std::vector<const TypeInfo*> arg_types;
		};

		////////////////////////
		///   Constructors   ///
	public:

		ConstructorInfo(Data data)
			: _data(std::move(data))
		{
		}

		///////////////////
		///   Methods   ///
	public:

		/**
		 * \brief Returns the number and type of all arguments required for this constructor.
		 */
		const std::vector<const TypeInfo*>& arg_types() const
		{
			return _data.arg_types;
		}

		/**
		 * \brief Invokes this constructor
		 * \param addr The address to allocate this object at
		 * \param args The array of arguments for this constructor (must match length and types described in 'arg_types')
		 */
		virtual void invoke(void* addr, const ArgAny* args) const = 0;

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
