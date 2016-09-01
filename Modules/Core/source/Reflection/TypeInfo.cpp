// TypeInfo.cpp

#include "../../include/Core/Reflection/TypeInfo.h"

namespace sge
{
	////////////////////////
	///   Constructors   ///

	TypeInfo::TypeInfo()
	{
		size = 0;
		alignment = 0;
		init = nullptr;
		copy_init = nullptr;
		move_init = nullptr;
		copy_assign = nullptr;
		move_assign = nullptr;
		drop = nullptr;
		base = nullptr;
	}
}
