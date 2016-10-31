// CStaticMesh.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CStaticMesh.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CStaticMesh)
.implements<IToArchive>()
.implements<IFromArchive>()
.property("mesh", component_getter(CStaticMesh::get_mesh), component_setter(CStaticMesh::set_mesh))
.property("material", component_getter(CStaticMesh::get_material), component_setter(CStaticMesh::set_material))
.field("mesh", &CStaticMesh::_mesh)
.field("material", &CStaticMesh::_material);

SGE_REFLECT_TYPE(sge::CStaticMesh::TMeshChanged);
SGE_REFLECT_TYPE(sge::CStaticMeshOverrideMaterial);
SGE_REFLECT_TYPE(sge::CStaticMeshMaterialOverrideParameters);
SGE_REFLECT_TYPE(sge::CStaticMeshMaterialOverrideParameters::TParamChanged);

namespace sge
{
	std::string CStaticMesh::get_mesh(TComponentInstance<const CStaticMesh> self)
	{
		return self->_mesh;
	}

	void CStaticMesh::set_mesh(TComponentInstance<CStaticMesh> self, Frame& frame, std::string mesh)
	{
		self->_mesh = std::move(mesh);
	}

	std::string CStaticMesh::get_material(TComponentInstance<const CStaticMesh> self)
	{
		return self->_mesh;
	}

	void CStaticMesh::set_material(TComponentInstance<CStaticMesh> self, Frame& frame, std::string material)
	{
		self->_material = std::move(material);
	}

}