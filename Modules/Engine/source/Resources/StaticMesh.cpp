// StaticMesh.cpp

#include <fstream>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Resource/Interfaces/IFromFile.h>
#include "../../include/Engine/Resources/StaticMesh.h"
#include "Resource/Archives/BinaryArchive.h"

SGE_REFLECT_TYPE(sge::StaticMesh)
.flags(TF_SCRIPT_NOCONSTRUCT)
.implements<IToArchive>()
.implements<IFromArchive>()
.implements<IFromFile>();

namespace sge
{
	StaticMesh::StaticMesh()
	{
	}

	void StaticMesh::to_archive(ArchiveWriter& writer) const
	{
		// Write vertex positions
		writer.push_object_member("vpos");
		writer.typed_array(_vertex_positions.data()->vec(), _vertex_positions.size() * 3);
		writer.pop();

		// Write vertex normals
		writer.push_object_member("vnor");
		writer.typed_array(_vertex_normals.data()->vec(), _vertex_normals.size() * 3);
		writer.pop();

        // Write vertex tangents
        writer.push_object_member("vtan");
        writer.typed_array(_vertex_tangents.data()->vec(), _vertex_tangents.size() * 3);
        writer.pop();

		// Write the first UV layer
		writer.push_object_member("uv0");
		writer.typed_array(_uv_map_0.data()->vec(), _uv_map_0.size() * 2);
		writer.pop();
	}

    void StaticMesh::from_archive(ArchiveReader& reader)
    {
        // Reset data
        _vertex_positions.clear();
        _vertex_normals.clear();
        _vertex_tangents.clear();
        _uv_map_0.clear();

        // Enumerate members (better than re-searching)
        reader.enumerate_object_members([this, &reader](const char* name)
        {
            if (std::strcmp(name, "vpos") == 0)
            {
                // Get vertex positions
                std::size_t size = 0;
                reader.array_size(size);
                this->_vertex_positions.assign(size / 3, Vec3::zero());
                reader.typed_array(this->_vertex_positions.data()->vec(), size);
            }
            else if (std::strcmp(name, "vnor") == 0)
            {
                // Get vertex normals
                std::size_t size = 0;
                reader.array_size(size);
                this->_vertex_normals.assign(size / 3, Vec3::zero());
                reader.typed_array(this->_vertex_normals.data()->vec(), size);
            }
            else if (std::strcmp(name, "vtan") == 0)
            {
                // Get vertex tangents
                std::size_t size = 0;
                reader.array_size(size);
                this->_vertex_tangents.assign(size / 3, Vec3::zero());
                reader.typed_array(this->_vertex_tangents.data()->vec(), size);
            }
            else if (std::strcmp(name, "uv0") == 0)
            {
                // Get first uv layer
                std::size_t size = 0;
                reader.array_size(size);
                this->_uv_map_0.assign(size / 2, Vec2::zero());
                reader.typed_array(this->_uv_map_0.data()->vec(), size);
            }
        });
    }

    bool StaticMesh::from_file(const char* path)
    {
        BinaryArchive bin;
        if (!bin.from_file(path))
        {
            return false;
        }

        auto* reader = bin.read_root();
        from_archive(*reader);
        reader->pop();
        return true;
    }
}
