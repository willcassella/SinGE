// StaticMesh.cpp

#include "../../../include/Engine/Components/Display/StaticMesh.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CStaticMesh);
SGE_REFLECT_TYPE(sge::CStaticMesh::TMeshChanged);
SGE_REFLECT_TYPE(sge::CStaticMeshOverrideMaterial);
SGE_REFLECT_TYPE(sge::CStaticMeshMaterialOverrideParameters);

namespace sge
{
	void CStaticMesh::set_mesh(ComponentInstance<CStaticMesh> self, Scene& scene, std::string mesh)
	{
		self.object->_mesh = std::move(mesh);
	}
}