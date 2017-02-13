// CStaticMesh.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CStaticMesh.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/TagBuffer.h"

SGE_REFLECT_TYPE(sge::CStaticMesh)
.property("mesh", &CStaticMesh::mesh, &CStaticMesh::mesh)
.property("material", &CStaticMesh::material, &CStaticMesh::material);

SGE_REFLECT_TYPE(sge::CStaticMesh::FMeshChanged);
SGE_REFLECT_TYPE(sge::CStaticMesh::FMaterialChanged);
SGE_REFLECT_TYPE(sge::CStaticMeshMaterialOverrideParameters);

namespace sge
{
	struct CStaticMesh::Data
	{
		///////////////////
		///   Methods   ///
	public:

		void to_archive(ArchiveWriter& writer) const
		{
			writer.object_member("mesh", mesh);
			writer.object_member("material", material);
		}

		void from_archive(ArchiveReader& reader)
		{
			reader.object_member("mesh", mesh);
			reader.object_member("material", material);
		}

		//////////////////
		///   Fields   ///
	public:

		std::string mesh;
		std::string material;
	};

	void CStaticMesh::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CStaticMesh, Data>>());
	}

    void CStaticMesh::reset(Data& data)
	{
        _data = &data;
	}

	const std::string& CStaticMesh::mesh() const
	{
		return _data->mesh;
	}

	void CStaticMesh::mesh(std::string mesh)
	{
        if (mesh != _data->mesh)
        {
		    _data->mesh = std::move(mesh);
            _mesh_changed.add_single_tag(entity(), FMeshChanged{});
            set_modified();
        }
	}

	const std::string& CStaticMesh::material() const
	{
		return _data->material;
	}

	void CStaticMesh::material(std::string material)
	{
        if (material != _data->material)
        {
		    _data->material = std::move(material);
            _material_changed.add_single_tag(entity(), FMaterialChanged{});
            set_modified();
        }
	}

    void CStaticMesh::generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags)
    {
        // Call the base implementation
        ComponentInterface::generate_tags(tags);

        // Add tags
        _mesh_changed.create_buffer(&type_info, tags);
        _material_changed.create_buffer(&type_info, tags);
    }
}
