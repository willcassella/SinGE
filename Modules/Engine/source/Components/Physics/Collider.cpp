// Collider.cpp

#include "../../../include/Engine/Components/Physics/Collider.h"
#include "../../../include/Engine/Component.h"

REFLECT_TYPE(singe::Collider)
.auto_impl<singe::Component>();

namespace singe
{
	////////////////////////
	///   Constructors   ///

	Collider::Collider()
	{
	}
}
