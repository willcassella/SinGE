// StaticMesh.h
#pragma once

#include <string>
#include <unordered_map>
#include <Core/Math/Vec2.h>
#include "../../Scene.h"

namespace sge
{
	struct SGE_ENGINE_API CStaticMesh
	{
		SGE_REFLECTED_TYPE;
	
		//////////////////
		///   Fields   ///
	public:

		std::string mesh;
		std::string material;
		std::unordered_map<std::string, float> float_params;
		std::unordered_map<std::string, Vec2> vec2_params;

		////////////////
		///   Tags   ///
	public:

		struct SGE_ENGINE_API TMeshChanged
		{
			SGE_REFLECTED_TYPE;
		};

		struct SGE_ENGINE_API TMaterialChanged
		{
			SGE_REFLECTED_TYPE;
		};

		struct SGE_ENGINE_API TParamsChanged
		{
			SGE_REFLECTED_TYPE;
		};

		/////////////////////
		///   Functions   ///
	public:

		static void set_mesh(ComponentInstance<CStaticMesh> self, Scene& scene, std::string mesh);

		static void set_material(ComponentInstance<CStaticMesh> self, Scene& scene, std::string material);

		static void set_param(ComponentInstance<CStaticMesh> self, Scene& scene, std::string name, float value);

		static void set_param(ComponentInstance<CStaticMesh> self, Scene& scene, std::string name, Vec2 value);
	};
}
