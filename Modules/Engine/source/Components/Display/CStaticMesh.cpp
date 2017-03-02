// CStaticMesh.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Math/IVec2.h>
#include <Resource/Resources/StaticMesh.h>
#include "../../../include/Engine/Components/Display/CStaticMesh.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/TagBuffer.h"

SGE_REFLECT_TYPE(sge::CStaticMesh)
.property("mesh", &CStaticMesh::mesh, &CStaticMesh::mesh)
.property("material", &CStaticMesh::material, &CStaticMesh::material)
.property("use_lightmap", &CStaticMesh::uses_lightmap, &CStaticMesh::set_uses_lightmap)
.property("lightmap_width", &CStaticMesh::lightmap_width, &CStaticMesh::lightmap_width)
.property("lightmap_height", &CStaticMesh::lightmap_height, &CStaticMesh::lightmap_height);

SGE_REFLECT_TYPE(sge::CStaticMesh::FMeshChanged);
SGE_REFLECT_TYPE(sge::CStaticMesh::FMaterialChanged);
SGE_REFLECT_TYPE(sge::CStaticMesh::FLightmapChanged);

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
            writer.object_member("uselm", use_lightmap);
            writer.object_member("lm", lightmap);
            writer.object_member("lms", lightmap_size);
		}

		void from_archive(ArchiveReader& reader)
		{
			reader.object_member("mesh", mesh);
			reader.object_member("material", material);
            reader.object_member("uselm", use_lightmap);
            reader.object_member("lm", lightmap);
            reader.object_member("lms", lightmap_size);
		}

		//////////////////
		///   Fields   ///
	public:

        /**
         * \brief Path to the mesh.
         */
		std::string mesh;

	    /**
	     * \brief Path to the material being used for this mesh.
	     */
	    std::string material;

	    /**
         * \brief Whether this mesh should use a lightmap.
         */
        bool use_lightmap = false;

	    /**
         * \brief Path to the lightmap used for this mesh.
         */
        std::string lightmap;

	    /**
         * \brief Lightmap resolution for this mesh.
         */
        IVec2<int32> lightmap_size = { 512, 512 };
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

    bool CStaticMesh::uses_lightmap() const
    {
        return _data->use_lightmap;
    }

    void CStaticMesh::set_uses_lightmap(bool value)
    {
        checked_setter(value, _data->use_lightmap);
    }

    const std::string& CStaticMesh::lightmap() const
    {
        return _data->lightmap;
    }

    void CStaticMesh::lightmap(std::string lightmap)
    {
        if (_data->lightmap != lightmap)
        {
            _data->lightmap = std::move(lightmap);
            _lightmap_changed.add_single_tag(entity(), FLightmapChanged{});
        }
    }

    int32 CStaticMesh::lightmap_width() const
    {
        return _data->lightmap_size.x();
    }

    void CStaticMesh::lightmap_width(int32 width)
    {
        if (lightmap_width() != width)
        {
            _data->lightmap_size.x(width);
            set_modified();
        }
    }

    void CStaticMesh::lightmap_height(int32 height)
    {
        if (lightmap_height() != height)
        {
            _data->lightmap_size.y(height);
            set_modified();
        }
    }

    int32 CStaticMesh::lightmap_height() const
    {
        return _data->lightmap_size.y();
    }

    void CStaticMesh::generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags)
    {
        // Call the base implementation
        ComponentInterface::generate_tags(tags);

        // Add tags
        _mesh_changed.create_buffer(&type_info, tags);
        _material_changed.create_buffer(&type_info, tags);
        _lightmap_changed.create_buffer(&type_info, tags);
    }
}
