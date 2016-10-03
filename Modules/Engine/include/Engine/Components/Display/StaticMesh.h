// StaticMesh.h
#pragma once

#include <string>
#include <unordered_map>
#include <Core/Math/Vec2.h>
#include <Core/Math/Vec4.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CStaticMesh
	{
		SGE_REFLECTED_TYPE;

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

		static void set_mesh(TComponentInstance<CStaticMesh> self, Frame& frame, std::string mesh);

		//////////////////
		///   Fields   ///
	private:

		std::string _mesh;
	};

	struct SGE_ENGINE_API CStaticMeshOverrideMaterial
	{
		SGE_REFLECTED_TYPE;
		std::string material;
	};

	struct SGE_ENGINE_API CStaticMeshMaterialOverrideParameters
	{
		SGE_REFLECTED_TYPE;

		struct SGE_ENGINE_API TParamChanged
		{
			SGE_REFLECTED_TYPE;
		};

	public:

		/* Sets a float param value. */
		static void set_param(
			TComponentInstance<CStaticMeshMaterialOverrideParameters> self,
			Frame& frame,
			const char* name,
			float value);

		/* Sets a Vec2 param value. */
		static void set_param(
			TComponentInstance<CStaticMeshMaterialOverrideParameters> self,
			Frame& frame,
			const char* name,
			Vec2 value);

		/* Sets a Vec3 param value. */
		static void set_param(
			TComponentInstance<CStaticMeshMaterialOverrideParameters> self,
			Frame& frame,
			const char* name,
			Vec3 value);

		/* Sets a Vec4 param value. */
		static void set_param(
			TComponentInstance<CStaticMeshMaterialOverrideParameters> self,
			Frame& frame,
			const char* name,
			Vec4 value);

		/* Sets a texture param value. */
		static void set_param(
			TComponentInstance<CStaticMeshMaterialOverrideParameters> self,
			Frame& frame,
			const char* name,
			std::string value);

	private:

		std::unordered_map<std::string, float> _float_params;
		std::unordered_map<std::string, Vec2> _vec2_params;
		std::unordered_map<std::string, Vec3> _vec3_params;
		std::unordered_map<std::string, Vec4> _vec4_params;
		std::unordered_map<std::string, std::string> _texture_params;
	};
}
