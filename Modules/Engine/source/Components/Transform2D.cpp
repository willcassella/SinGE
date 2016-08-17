// Transform2D.cpp

#include <Core/Interfaces/IToString.h>
#include "../../include/Engine/Components/Transform2D.h"
#include "../../include/Engine/Component.h"

REFLECT_TYPE(singe::Transform2D)
.auto_impl<singe::IToString>()
.auto_impl<singe::IComponent>();

namespace singe
{
	////////////////////////
	///   Constructors   ///

	Transform2D::Transform2D()
		: rot{ 0 }
	{
	}

	///////////////////
	///   Methods   ///

	std::string Transform2D::to_string() const
	{
		return "Transform";
	}
}
