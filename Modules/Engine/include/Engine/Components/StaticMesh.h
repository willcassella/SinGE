// StaticMesh.h
#pragma once

#include <string>
#include <Core/Reflection/Reflection.h>
#include "../config.h"

namespace singe
{
	struct ENGINE_API StaticMesh
	{
		REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		StaticMesh();

		//////////////////
		///   Fields   ///
	public:

		std::string mesh;
		std::string material;
		bool visible;
	};
}
