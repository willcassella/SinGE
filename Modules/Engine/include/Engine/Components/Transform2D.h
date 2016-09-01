// Transform2D.h
#pragma once

#include <Core/Math/Vec2.h>
#include "../config.h"

namespace sge
{
	struct SGE_ENGINE_API CTransform2D
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		CTransform2D();

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

		////////////////
		///   Tags   ///
	public:

		struct SGE_ENGINE_API TChanged
		{
			SGE_REFLECTED_TYPE;
		};
	};
}
