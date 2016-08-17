// Vec2.h
#pragma once

#include <string>
#include <sstream>
#include "../Reflection/Reflection.h"

namespace singe
{
	struct CORE_API Vec2 final
	{
		REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		Vec2()
			: x{ 0 }, y{ 0 }
		{
		}
		Vec2(float x, float y)
			: x{ x }, y{ y }
		{
		}

		//////////////////
		///   Fields   ///
	public:

		float x;
		float y;

		///////////////////
		///   Methods   ///
	public:

		std::string to_string() const;
	};
}
