// TypeInfo.cpp

#include "../../include/Core/Reflection/TypeInfo.h"

namespace singe
{
	////////////////////////
	///   Constructors   ///

	TypeInfo::TypeInfo()
	{
		size = 0;
		alignment = 0;
		initialize = nullptr;
		copy_initialize = nullptr;
		move_initialize = nullptr;
		copy_assign = nullptr;
		move_assign = nullptr;
		drop = nullptr;
		base = nullptr;
	}
}
