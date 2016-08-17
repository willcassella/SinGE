// Transform2D.h
#pragma once

#include <Core/Math/Vec2.h>
#include "../config.h"

namespace singe
{
	struct ENGINE_API Transform2D final
	{
		REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		Transform2D();

		///////////////////
		///   Fields    ///
	public:

		Vec2 position;
		Vec2 scale;
		float rot;
		
		///////////////////
		///   Methods   ///
	public:

		std::string to_string() const;
	};
}
