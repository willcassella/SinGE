// IToString.h
#pragma once

#include <string>
#include "../Reflection/Reflection.h"

namespace singe
{
	struct CORE_API IToString final
	{
		REFLECTED_INTERFACE;
		AUTO_IMPL_1(IToString, to_string)

		/////////////////////
		///   Functions   ///
	public:

		std::string(C_CALL*to_string)(const void* self);
	};
}
