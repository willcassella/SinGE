// Vec2.cpp

#include <sstream>
#include "../../include/Core/Math/Vec2.h"
#include "../../include/Core/Interfaces/IToString.h"

SGE_REFLECT_TYPE(sge::Vec2)
.implements<sge::IToString>();

namespace sge
{
	std::string Vec2::to_string() const
	{
		std::stringstream out;
		out << "{ " << x << ", " << y << " }";
		return out.str();
	}
}
