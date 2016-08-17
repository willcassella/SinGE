// CubeCollider.h
#pragma once

#include "Collider.h"

namespace singe
{
	struct ENGINE_API CubeCollider final : Collider
	{
		REFLECTED_TYPE;

		////////////////
		///   Data   ///
	private:

		Vec2 _shape;
	};
}
