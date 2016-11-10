// CStaticMesh.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CStaticMesh.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CStaticMesh)
.property("mesh", &CStaticMesh::mesh, &CStaticMesh::mesh)
.property("material", &CStaticMesh::material, &CStaticMesh::material);

SGE_REFLECT_TYPE(sge::CStaticMesh::FMeshChanged);
SGE_REFLECT_TYPE(sge::CStaticMesh::FMaterialChanged);
SGE_REFLECT_TYPE(sge::CStaticMeshOverrideMaterial);
SGE_REFLECT_TYPE(sge::CStaticMeshMaterialOverrideParameters);
SGE_REFLECT_TYPE(sge::CStaticMeshMaterialOverrideParameters::TParamChanged);

namespace sge
{
	struct CStaticMesh::Data
	{
		///////////////////
		///   Methods   ///
	public:

		void to_archive(ArchiveWriter& writer) const
		{
			writer.push_object_member("mesh", mesh);
			writer.push_object_member("material", material);
		}

		void from_archive(const ArchiveReader& reader)
		{
			reader.pull_object_member("mesh", mesh);
			reader.pull_object_member("material", material);
		}

		//////////////////
		///   Fields   ///
	public:

		std::string mesh;
		std::string material;
	};

	CStaticMesh::CStaticMesh(ProcessingFrame& pframe, EntityId entity, Data& data)
		: TComponentInterface<sge::CStaticMesh>(pframe, entity),
		_data(&data)
	{
	}

	void CStaticMesh::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CStaticMesh, Data>>());
	}

	const std::string& CStaticMesh::mesh() const
	{
		return _data->mesh;
	}

	void CStaticMesh::mesh(std::string mesh)
	{
		_data->mesh = std::move(mesh);
	}

	const std::string& CStaticMesh::material() const
	{
		return _data->material;
	}

	void CStaticMesh::material(std::string material)
	{
		_data->material = std::move(material);
	}
}