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
        _current_changed_mesh = false;
        _current_changed_material = false;
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

            if (!_current_changed_mesh)
            {
                _current_changed_mesh = true;
                _ord_changed_meshes.push_back(entity());
            }
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

            if (!_current_changed_material)
            {
                _current_changed_material = true;
                _ord_changed_materials.push_back(entity());
            }
        }
	}

    void CStaticMesh::generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags)
    {
        // Call the base implementation
        ComponentInterface::generate_tags(tags);

        // Generate the changed mesh tags
        if (!_ord_changed_meshes.empty())
        {
            FMeshChanged mesh_tag;
            tags[&FMeshChanged::type_info].push_back(TagBuffer::create_from_single(
                type_info,
                _ord_changed_meshes.data(),
                &mesh_tag,
                sizeof(FMeshChanged),
                _ord_changed_meshes.size()));
        }

        // Generate the changed material tags
        if (!_ord_changed_materials.empty())
        {
	        FMaterialChanged mat_tag;
            tags[&FMaterialChanged::type_info].push_back(TagBuffer::create_from_single(
                type_info,
                _ord_changed_materials.data(),
                &mat_tag,
                sizeof(FMaterialChanged),
                _ord_changed_materials.size()));
        }
    }
}
