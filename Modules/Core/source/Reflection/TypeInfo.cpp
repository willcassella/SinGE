// TypeInfo.cpp

#include "../../include/Core/Reflection/TypeInfo.h"
#include "../../include/Core/Reflection/FieldInfo.h"
#include "../../include/Core/Reflection/PropertyInfo.h"

namespace sge
{
	TypeInfo::Data::Data()
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
