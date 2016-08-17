// Vec2.cpp

#include <sstream>
#include "../../include/Core/Math/Vec2.h"
#include "../../include/Core/Interfaces/IToString.h"

REFLECT_TYPE(singe::Vec2)
.auto_impl<singe::IToString>();

namespace singe
{
	std::string Vec2::to_string() const
	{
		std::stringstream out;
		out << "{ " << x << ", " << y << " }";
		return out.str();
	}
}
