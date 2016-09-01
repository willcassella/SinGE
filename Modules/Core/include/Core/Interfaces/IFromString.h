// IFromString.h
#pragma once

#include <string>
#include "../Reflection/Reflection.h"

namespace sge
{
	struct SGE_CORE_API IFromString
	{
		SGE_REFLECTED_INTERFACE;
		SGE_VTABLE_1(IFromString, from_string)

		/////////////////////
		///   Functions   ///
	public:

		std::string(SGE_C_CALL*from_string)(void* self, const std::string& str);
	};

	template <typename T>
	struct Impl< IFromString, T >
	{
		static std::string from_string(void* self, const std::string& str)
		{
			return static_cast<T*>(self)->from_string(str);
		}
	};

	template <typename T>
	std::string from_string(T& self, const std::string& str)
	{
		return Impl<IFromString, T>::from_string(&self, str);
	}
}
