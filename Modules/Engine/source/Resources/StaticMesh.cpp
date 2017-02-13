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
    void StaticMesh::SubMesh::serialize(ArchiveWriter& writer) const
    {
        // Write default material
        writer.object_member("mat", _default_material);

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

        // Write bitangent signs
        writer.push_object_member("vbts");
        writer.typed_array(_bitangent_signs.data(), _bitangent_signs.size());
        writer.pop();

        // Write the material UV layer
        writer.push_object_member("uv");
        writer.typed_array(_material_uv.data()->vec(), _material_uv.size() * 2);
        writer.pop();

        // Write vertex indices
        writer.push_object_member("ind");
        writer.typed_array(_triangle_elements.data(), _triangle_elements.size());
        writer.pop();
    }

    void StaticMesh::SubMesh::deserialize(ArchiveReader& reader, std::string name)
    {
        _name = std::move(name);
        _default_material.clear();
        _vertex_positions.clear();
        _vertex_normals.clear();
        _vertex_tangents.clear();
        _bitangent_signs.clear();
        _material_uv.clear();
        _triangle_elements.clear();

        std::size_t num_verts = 0;
        reader.enumerate_object_members([this, &reader, &num_verts](const char* mem_name)
        {
            if (std::strcmp(mem_name, "mat") == 0)
            {
                // Get the default material name
                assert(reader.is_string());
                sge::from_archive(this->_default_material, reader);
            }
            else if (std::strcmp(mem_name, "vpos") == 0)
            {
                // Get vertex positions array size
                std::size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert(got_size && num_verts == 0 || num_verts == size / 3);
                num_verts = size / 3;

                // Get vertex positions array
                this->_vertex_positions.assign(size / 3, Vec3::zero());
                const auto read_size = reader.typed_array(this->_vertex_positions.data()->vec(), size);
                assert(read_size == size);
            }
            else if (std::strcmp(mem_name, "vnor") == 0)
            {
                // Get vertex normals array size
                std::size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert(got_size && num_verts == 0 || num_verts == size / 3);
                num_verts = size / 3;

                // Get vertex normals array
                this->_vertex_normals.assign(size / 3, HalfVec3::zero());
                const auto read_size = reader.typed_array(this->_vertex_normals.data()->vec(), size);
                assert(read_size == size);
            }
            else if (std::strcmp(mem_name, "vtan") == 0)
            {
                // Get vertex tangents array size
                std::size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert(got_size && num_verts == 0 || size == num_verts * 3);
                num_verts = size / 3;

                // Get vertex tangents
                this->_vertex_tangents.assign(size / 3, HalfVec3::zero());
                const auto read_size = reader.typed_array(this->_vertex_tangents.data()->vec(), size);
                assert(read_size == size);
            }
            else if (std::strcmp(mem_name, "vbts") == 0)
            {
                // Get vertex bitangent signs array size
                std::size_t size = 0;
                const bool got_size = reader.array_size(size);
                assert(got_size && num_verts == 0 || size == num_verts);
                num_verts = size;

                // Get vertex bitangent signs
                this->_bitangent_signs.assign(size, 0);
                const auto read_size = reader.typed_array(this->_bitangent_signs.data(), size);
                assert(read_size == size);
            }
            else if (std::strcmp(mem_name, "uv") == 0)
            {
                // Get uv array size
                std::size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert(got_size && num_verts == 0 || size == num_verts * 2);
                num_verts = size / 2;

                // Get uvs
                this->_material_uv.assign(size / 2, HalfVec2::zero());
                const auto read_size = reader.typed_array(this->_material_uv.data()->vec(), size);
                assert(read_size == size);
            }
            else if (std::strcmp(mem_name, "ind") == 0)
            {
                // Get element array size
                std::size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert(got_size);

                // Get elements
                this->_triangle_elements.assign(size, 0);
                const auto read_size = reader.typed_array(this->_triangle_elements.data(), size);
                assert(read_size == size);
            }
        });
    }

    const std::string& StaticMesh::SubMesh::name() const
    {
        return _name;
    }

    const std::string& StaticMesh::SubMesh::default_material() const
    {
        return _default_material;
    }

    std::size_t StaticMesh::SubMesh::num_verts() const
    {
        return _vertex_positions.size();
    }

    const Vec3* StaticMesh::SubMesh::vertex_positions() const
    {
        if (_vertex_positions.empty())
        {
            return nullptr;
        }

        return _vertex_positions.data();
    }

    const HalfVec3* StaticMesh::SubMesh::vertex_normals() const
    {
        if (_vertex_normals.empty())
        {
            return nullptr;
        }

        return _vertex_normals.data();
    }

    const HalfVec3* StaticMesh::SubMesh::vertex_tangents() const
    {
        if (_vertex_tangents.empty())
        {
            return nullptr;
        }

        return _vertex_tangents.data();
    }

    const int8* StaticMesh::SubMesh::bitangent_signs() const
    {
        if (_bitangent_signs.empty())
        {
            return nullptr;
        }

        return _bitangent_signs.data();
    }

    const HalfVec2* StaticMesh::SubMesh::material_uv() const
    {
        if (_material_uv.empty())
        {
            return nullptr;
        }

        return _material_uv.data();
    }

    std::size_t StaticMesh::SubMesh::num_triangles() const
    {
        return _triangle_elements.size() / 3;
    }

    std::size_t StaticMesh::SubMesh::num_triangle_elements() const
    {
        return _triangle_elements.size();
    }

    const uint32* StaticMesh::SubMesh::triangle_elements() const
    {
        if (_triangle_elements.empty())
        {
            return nullptr;
        }

        return _triangle_elements.data();
    }

    StaticMesh::StaticMesh()
	{
	}

	void StaticMesh::to_archive(ArchiveWriter& writer) const
	{
        writer.push_object_member("objs");

        // Serialize subobjects
        for (const auto& sub_obj : _sub_meshes)
        {
            writer.push_object_member(sub_obj.name().c_str());
            sub_obj.serialize(writer);
            writer.pop();
        }

        writer.pop();
	}

    void StaticMesh::from_archive(ArchiveReader& reader)
    {
        _sub_meshes.clear();

        if (reader.pull_object_member("objs"))
        {
            reader.enumerate_object_members([this, &reader](const char* mem_name)
            {
                // Serialize a submesh
                SubMesh sub_mesh;
                sub_mesh.deserialize(reader, mem_name);

                _sub_meshes.push_back(std::move(sub_mesh));
            });

            reader.pop();
        }
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

    std::size_t StaticMesh::num_sub_meshes() const
    {
        return _sub_meshes.size();
    }

    const StaticMesh::SubMesh* StaticMesh::sub_meshes() const
    {
        if (_sub_meshes.empty())
        {
            return nullptr;
        }

        return _sub_meshes.data();
    }
}
