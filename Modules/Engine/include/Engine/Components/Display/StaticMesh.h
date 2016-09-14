// StaticMesh.h
#pragma once

#include <string>
#include <unordered_map>
#include <Core/Math/Vec2.h>
#include <Core/Math/Vec4.h>
#include "../../config.h"

namespace sge
{
	struct Scene;

	template <class T>
	struct ComponentInstance;

	struct SGE_ENGINE_API CStaticMesh
	{
		SGE_REFLECTED_TYPE;
	
		////////////////
		///   Data   ///
	private:

		std::string _mesh;

		///////////////////
		///   Methods   ///
	public:

		std::string get_mesh() const
		{
			return _mesh;
		}

		////////////////
		///   Tags   ///
	public:

		struct SGE_ENGINE_API TMeshChanged
		{
			SGE_REFLECTED_TYPE;
		};

		/////////////////////
		///   Functions   ///
	public:

		static void set_mesh(ComponentInstance<CStaticMesh> self, Scene& scene, std::string mesh);
	};

	struct SGE_ENGINE_API CStaticMeshOverrideMaterial
	{
		SGE_REFLECTED_TYPE;
		std::string material;
	};

	struct SGE_ENGINE_API CStaticMeshMaterialOverrideParameters
	{
		SGE_REFLECTED_TYPE;
		std::unordered_map<std::string, float> _float_params;
		std::unordered_map<std::string, Vec2> _vec2_params;
		std::unordered_map<std::string, Vec3> _vec3_params;
		std::unordered_map<std::string, Vec4> _vec4_params;
		std::unordered_map<std::string, std::string> _texture_params;
	};
}
