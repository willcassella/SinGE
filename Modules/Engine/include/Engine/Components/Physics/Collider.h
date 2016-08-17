// Collider.h
#pragma once

#include <Core/Math/Vec2.h>
#include "../../config.h"

namespace singe
{
	struct ENGINE_API Collider
	{
		REFLECTED_TYPE;
		
		////////////////////////
		///   Constructors   ///
	protected:

		Collider();

		///////////////////
		///   Methods   ///
	public:

		Vec2 get_offset() const
		{
			return _offset;
		}

		////////////////
		///   Data   ///
	private:

		Vec2 _offset;
	};
};
