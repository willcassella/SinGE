// Transform2D.cpp

#include <Core/Interfaces/IToString.h>
#include "../../include/Engine/Components/Transform2D.h"
#include "../../include/Engine/Component.h"

SGE_REFLECT_TYPE(sge::CTransform2D)
.implements<sge::IToString>();

SGE_REFLECT_TYPE(sge::CTransform2D::TChanged);

namespace sge
{
	////////////////////////
	///   Constructors   ///

	CTransform2D::CTransform2D()
		: rot{ 0 }
	{
	}

	///////////////////
	///   Methods   ///

	std::string CTransform2D::to_string() const
	{
		return "Transform";
	}
}
