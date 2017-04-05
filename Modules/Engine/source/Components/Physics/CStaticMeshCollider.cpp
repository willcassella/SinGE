// CStaticMeshCollider.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Physics/CStaticMeshCollider.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/CSharedData.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"

SGE_REFLECT_TYPE(sge::CStaticMeshCollider)
.implements<IToArchive>()
.implements<IFromArchive>()
.property("mesh", &CStaticMeshCollider::mesh, &CStaticMeshCollider::mesh);

namespace sge
{
	struct CStaticMeshCollider::SharedData : CSharedData<CStaticMeshCollider>
	{
	};

	CStaticMeshCollider::CStaticMeshCollider(NodeId node, SharedData& shared_data)
		: _node(node),
		_shared_data(&shared_data)
	{
	}

	void CStaticMeshCollider::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CStaticMeshCollider, SharedData>>());
	}

	void CStaticMeshCollider::to_archive(ArchiveWriter& writer) const
	{
		writer.as_object();
		writer.object_member("mesh", _mesh);
	}

	void CStaticMeshCollider::from_archive(ArchiveReader& reader)
	{
		reader.object_member("mesh", _mesh);
	}

	NodeId CStaticMeshCollider::node() const
	{
		return _node;
	}

	const std::string& CStaticMeshCollider::mesh() const
	{
		return _mesh;
	}

	void CStaticMeshCollider::mesh(std::string value)
	{
		if (_mesh != value)
		{
			_mesh = std::move(value);
			_shared_data->set_modified(_node, this, "mesh");
		}
	}
}
