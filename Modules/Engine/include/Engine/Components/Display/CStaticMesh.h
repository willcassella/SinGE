// CStaticMesh.h
#pragma once

#include "../../Resources/Material.h"
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CStaticMesh
	{
		SGE_REFLECTED_TYPE;

		///////////////////
		///   Methods   ///
	public:

		const std::string& mesh() const
		{
			return _mesh;
		}

		const std::string& material() const
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

		/////////////////////
		///   Functions   ///
	public:

		static std::string get_mesh(TComponentInstance<const CStaticMesh> self);

		static void set_mesh(TComponentInstance<CStaticMesh> self, Frame& frame, std::string mesh);

		static std::string get_material(TComponentInstance<const CStaticMesh> self);

		static void set_material(TComponentInstance<CStaticMesh> self, Frame& frame, std::string material);

		//////////////////
		///   Fields   ///
	private:

		std::string _mesh;
		std::string _material;
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
			std::string name,
			float value);

		/* Sets a Vec2 param value. */
		static void set_param(
			TComponentInstance<CStaticMeshMaterialOverrideParameters> self,
			Frame& frame,
			std::string name,
			Vec2 value);

		/* Sets a Vec3 param value. */
		static void set_param(
			TComponentInstance<CStaticMeshMaterialOverrideParameters> self,
			Frame& frame,
			std::string name,
			Vec3 value);

		/* Sets a Vec4 param value. */
		static void set_param(
			TComponentInstance<CStaticMeshMaterialOverrideParameters> self,
			Frame& frame,
			std::string name,
			Vec4 value);

		/* Sets a texture param value. */
		static void set_param(
			TComponentInstance<CStaticMeshMaterialOverrideParameters> self,
			Frame& frame,
			std::string name,
			std::string value);

		//////////////////
		///   Fields   ///
	private:

		Material::ParamTable _param_table;
	};
}
