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
		std::string _material;
		std::unordered_map<std::string, float> _float_params;
		std::unordered_map<std::string, Vec2> _vec2_params;
		std::unordered_map<std::string, Vec3> _vec3_params;

		///////////////////
		///   Methods   ///
	public:

		std::string get_mesh() const
		{
			return _mesh;
		}

		std::string get_material() const
		{
			return _material;
		}

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
