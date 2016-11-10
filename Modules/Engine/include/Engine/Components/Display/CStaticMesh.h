// CStaticMesh.h
#pragma once

#include "../../Resources/Material.h"
#include "../../Component.h"

namespace sge
{
	/* Component applied to entities that should render a static mesh. */
	struct SGE_ENGINE_API CStaticMesh : TComponentInterface<CStaticMesh>
	{
		SGE_REFLECTED_TYPE;
		struct Data;

		////////////////////////
		///   Constructors   ///
	public:

		CStaticMesh(ProcessingFrame& pframe, EntityId entity, Data& data);

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

		const std::string& mesh() const;

		void mesh(std::string mesh);

		const std::string& material() const;

		void material(std::string material);

		////////////////
		///   Tags   ///
	public:

		struct SGE_ENGINE_API FMeshChanged
		{
			SGE_REFLECTED_TYPE;
		};

		struct SGE_ENGINE_API FMaterialChanged
		{
			SGE_REFLECTED_TYPE;
		};

		//////////////////
		///   Fields   ///
	private:

		Data* _data;
	};

	/* Component applied to entities with a 'CStaticMesh' component, to override the default material for that mesh. */
	struct SGE_ENGINE_API CStaticMeshOverrideMaterial : TComponentInterface<CStaticMeshOverrideMaterial>
	{
		SGE_REFLECTED_TYPE;

		//////////////////
		///   Fields   ///
	public:

		std::string material;
	};

	/* Component applied to entities with a 'CStaticMesh' component, to override the default material parameters for that mesh. */
	struct SGE_ENGINE_API CStaticMeshMaterialOverrideParameters : TComponentInterface<CStaticMeshMaterialOverrideParameters>
	{
		SGE_REFLECTED_TYPE;

		struct SGE_ENGINE_API TParamChanged
		{
			SGE_REFLECTED_TYPE;
		};

		///////////////////
		///   Methods   ///
	public:

		/* Sets a float param value. */
		void set_param(std::string name, float value);

		/* Sets a Vec2 param value. */
		void set_param(std::string name, Vec2 value);

		/* Sets a Vec3 param value. */
		void set_param(std::string name, Vec3 value);

		/* Sets a Vec4 param value. */
		void set_param(std::string name, Vec4 value);

		/* Sets a texture param value. */
		void set_param(std::string name, std::string value);

		//////////////////
		///   Fields   ///
	private:

		Material::ParamTable _param_table;
	};
}
