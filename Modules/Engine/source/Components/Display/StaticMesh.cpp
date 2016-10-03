// StaticMesh.cpp

#include "../../../include/Engine/Components/Display/StaticMesh.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CStaticMesh);
SGE_REFLECT_TYPE(sge::CStaticMesh::TMeshChanged);
SGE_REFLECT_TYPE(sge::CStaticMeshOverrideMaterial);
SGE_REFLECT_TYPE(sge::CStaticMeshMaterialOverrideParameters);
SGE_REFLECT_TYPE(sge::CStaticMeshMaterialOverrideParameters::TParamChanged);

namespace sge
{
	void CStaticMesh::set_mesh(TComponentInstance<CStaticMesh> self, Frame& frame, std::string mesh)
	{
		self->_mesh = std::move(mesh);
	}
}