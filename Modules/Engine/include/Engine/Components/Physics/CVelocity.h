// CVelocity.h
#pragma once

#include <Core/Math/Quat.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CVelocity
	{
		SGE_REFLECTED_TYPE;

		//////////////////
		///   Fields   ///
	private:

		Vec3 _linear_velocity;
		Quat _angular_velocity;
	};
}
