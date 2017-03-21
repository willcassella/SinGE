// CStaticMesh.cpp

#include <memory>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Math/IVec2.h>
#include <Resource/Resources/StaticMesh.h>
#include "../../../include/Engine/Components/Display/CStaticMesh.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"

SGE_REFLECT_TYPE(sge::CStaticMesh)
.property("mesh", &CStaticMesh::mesh, &CStaticMesh::mesh)
.property("material", &CStaticMesh::material, &CStaticMesh::material)
.property("use_lightmap", &CStaticMesh::uses_lightmap, &CStaticMesh::set_uses_lightmap)
.property("lightmap_width", &CStaticMesh::lightmap_width, &CStaticMesh::lightmap_width)
.property("lightmap_height", &CStaticMesh::lightmap_height, &CStaticMesh::lightmap_height);

namespace sge
{
	struct CStaticMesh::SharedData : CSharedData<CStaticMesh>
	{
	};

	CStaticMesh::CStaticMesh(NodeId node, SharedData& shared_data)
		: _node(node),
		_shared_data(&shared_data)
	{
	}

	void CStaticMesh::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CStaticMesh, SharedData>>());
	}

    void CStaticMesh::to_archive(ArchiveWriter& writer) const
    {
        writer.object_member("mesh", _mesh);
        writer.object_member("material", _material);
        writer.object_member("uselm", _use_lightmap);
        writer.object_member("lm", _lightmap);
        writer.object_member("lms", _lightmap_size);
    }

    void CStaticMesh::from_archive(ArchiveReader & reader)
    {
        reader.object_member("mesh", _mesh);
        reader.object_member("material", _material);
        reader.object_member("uselm", _use_lightmap);
        reader.object_member("lm", _lightmap);
        reader.object_member("lms", _lightmap_size);
    }

	const std::string& CStaticMesh::mesh() const
	{
		return _mesh;
	}

	void CStaticMesh::mesh(std::string mesh)
	{
        if (mesh != _mesh)
        {
		    _mesh = std::move(mesh);
			set_modified("mesh");
        }
	}

	const std::string& CStaticMesh::material() const
	{
		return _material;
	}

	void CStaticMesh::material(std::string material)
	{
        if (material != _material)
        {
		    _material = std::move(material);
            set_modified("material");
        }
	}

    bool CStaticMesh::uses_lightmap() const
    {
        return _use_lightmap;
    }

    void CStaticMesh::set_uses_lightmap(bool value)
    {
		if (_use_lightmap != value)
		{
			_use_lightmap = value;
			set_modified("use_lightmap");
		}
    }

    const std::string& CStaticMesh::lightmap() const
    {
        return _lightmap;
    }

    void CStaticMesh::lightmap(std::string lightmap)
    {
        if (_lightmap != lightmap)
        {
			_lightmap = std::move(lightmap);
			set_modified("lightmap");
        }
    }

    int32 CStaticMesh::lightmap_width() const
    {
        return _lightmap_size.x();
    }

    void CStaticMesh::lightmap_width(int32 width)
    {
        if (lightmap_width() != width)
        {
			_lightmap_size.x(width);
			set_modified("lightmap_width");
        }
    }

	int32 CStaticMesh::lightmap_height() const
	{
		return _lightmap_size.y();
	}

    void CStaticMesh::lightmap_height(int32 height)
    {
        if (lightmap_height() != height)
        {
			_lightmap_size.y(height);
			set_modified("lightmap_height");
        }
    }

	void CStaticMesh::set_modified(const char* property_name)
	{
		_shared_data->set_modified(_node, this, property_name);
	}
}
