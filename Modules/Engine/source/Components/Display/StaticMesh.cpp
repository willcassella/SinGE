// StaticMesh.cpp

#include "../../../include/Engine/Components/Display/StaticMesh.h"
#include "../../../include/Engine/Component.h"

SGE_REFLECT_TYPE(sge::CStaticMesh);
SGE_REFLECT_TYPE(sge::CStaticMesh::TMeshChanged);
SGE_REFLECT_TYPE(sge::CStaticMesh::TMaterialChanged);
SGE_REFLECT_TYPE(sge::CStaticMesh::TParamsChanged);

namespace sge
{
	void CStaticMesh::set_mesh(ComponentInstance<CStaticMesh> self, Scene& scene, std::string mesh)
	{
		self.object->mesh = std::move(mesh);
		scene.new_tag(self.id, CStaticMesh::TMeshChanged{});
	}

	void CStaticMesh::set_material(ComponentInstance<CStaticMesh> self, Scene& scene, std::string material)
	{
		self.object->material = std::move(material);
		scene.new_tag(self.id, CStaticMesh::TMaterialChanged{});
	}

	void CStaticMesh::set_param(ComponentInstance<CStaticMesh> self, Scene& scene, std::string name, float value)
	{
		self.object->float_params[std::move(name)] = value;
	}

	void CStaticMesh::set_param(ComponentInstance<CStaticMesh> self, Scene& scene, std::string name, Vec2 value)
	{
		self.object->vec2_params[std::move(name)] = value;
	}
}