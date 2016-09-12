// StaticMesh.cpp

#include "../../../include/Engine/Components/Display/StaticMesh.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CStaticMesh);
SGE_REFLECT_TYPE(sge::CStaticMesh::TMeshChanged);
SGE_REFLECT_TYPE(sge::CStaticMesh::TMaterialChanged);
SGE_REFLECT_TYPE(sge::CStaticMesh::TParamsChanged);

namespace sge
{
	void CStaticMesh::set_mesh(ComponentInstance<CStaticMesh> self, Scene& scene, std::string mesh)
	{
		self.object->_mesh = std::move(mesh);
	}

	void CStaticMesh::set_material(ComponentInstance<CStaticMesh> self, Scene& scene, std::string material)
	{
		self.object->_material = std::move(material);
	}

	void CStaticMesh::set_param(ComponentInstance<CStaticMesh> self, Scene& scene, std::string name, float value)
	{
		self.object->_float_params[std::move(name)] = value;
	}

	void CStaticMesh::set_param(ComponentInstance<CStaticMesh> self, Scene& scene, std::string name, Vec2 value)
	{
		self.object->_vec2_params[std::move(name)] = value;
	}
}