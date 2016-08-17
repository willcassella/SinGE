// IFromString.h
#pragma once

#include <string>
#include "../Reflection/Reflection.h"

namespace singe
{
	struct CORE_API IFromString final
	{
		REFLECTED_INTERFACE;
		AUTO_IMPL_1(IFromString, from_string)

		/////////////////////
		///   Functions   ///
	public:

		std::string(C_CALL*from_string)(void* self, const std::string& str);
	};
}
