#include <fstream>
#include <stdint.h>

#include "lib/base/reflection/reflection_builder.h"
#include "lib/resource/archives/binary_archive.h"
#include "lib/resource/interfaces/from_file.h"
#include "lib/resource/resources/static_mesh.h"

SGE_REFLECT_TYPE(sge::StaticMesh)
.flags(TF_SCRIPT_NOCONSTRUCT)
.implements<IToArchive>()
.implements<IFromArchive>()
.implements<IFromFile>();

namespace sge
{
    void StaticMesh::Material::to_archive(ArchiveWriter& writer) const
    {
        writer.object_member("path", _path);
        writer.object_member("estrt", _start_elem_index);
        writer.object_member("ecnt", _num_elem_indices);
    }

    void StaticMesh::Material::from_archive(ArchiveReader& reader)
    {
        _path = "";
        _start_elem_index = 0;
        _num_elem_indices = 0;

        reader.enumerate_object_members([this, &reader](const char* mem_name)
        {
            if (strcmp(mem_name, "path") == 0)
            {
                sge::from_archive(this->_path, reader);
            }
            else if (strcmp(mem_name, "estrt") == 0)
            {
                reader.number(this->_start_elem_index);
            }
            else if (strcmp(mem_name, "ecnt") == 0)
            {
                reader.number(this->_num_elem_indices);
            }
        });
    }

    const std::string& StaticMesh::Material::path() const
    {
        return _path;
    }

    uint32_t StaticMesh::Material::start_elem_index() const
    {
        return _start_elem_index;
    }

    uint32_t StaticMesh::Material::num_elem_indices() const
    {
        return _num_elem_indices;
    }

    StaticMesh::StaticMesh() = default;

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

        // Write bitangent signs
        writer.push_object_member("vbts");
        writer.typed_array(_bitangent_signs.data(), _bitangent_signs.size());
        writer.pop();

        // Write the material UV layer
        writer.push_object_member("mtuv");
        writer.typed_array(_material_uv.data()->vec(), _material_uv.size() * 2);
        writer.pop();

        // Write lightmap UV layer
        if (!_lightmap_uv.empty())
        {
            writer.push_object_member("lmuv");
            writer.typed_array(_lightmap_uv.data()->vec(), _lightmap_uv.size() * 2);
            writer.pop();
        }

        // Write triangle elements
        writer.push_object_member("elem");
        writer.typed_array(_triangle_elements.data(), _triangle_elements.size());
        writer.pop();

        // Write materials
        writer.push_object_member("mats");
        for (const auto& mat : _materials)
        {
            writer.push_array_element();
            mat.to_archive(writer);
            writer.pop();
        }
        writer.pop();
    }

    void StaticMesh::from_archive(ArchiveReader& reader)
    {
        _vertex_positions.clear();
        _vertex_normals.clear();
        _vertex_tangents.clear();
        _bitangent_signs.clear();
        _material_uv.clear();
        _lightmap_uv.clear();
        _triangle_elements.clear();
        _materials.clear();

        size_t num_verts = 0;
        reader.enumerate_object_members([this, &reader, &num_verts](const char* mem_name)
        {
            if (strcmp(mem_name, "vpos") == 0)
            {
                // Get vertex positions array size
                size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert((got_size && num_verts == 0) || num_verts == size / 3);
                num_verts = size / 3;

                // Get vertex positions array
                this->_vertex_positions.assign(size / 3, Vec3::zero());
                const auto read_size = reader.typed_array(this->_vertex_positions.data()->vec(), size);
                assert(read_size == size);
            }
            else if (strcmp(mem_name, "vnor") == 0)
            {
                // Get vertex normals array size
                size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert((got_size && num_verts == 0) || num_verts == size / 3);
                num_verts = size / 3;

                // Get vertex normals array
                this->_vertex_normals.assign(size / 3, HalfVec3::zero());
                const auto read_size = reader.typed_array(this->_vertex_normals.data()->vec(), size);
                assert(read_size == size);
            }
            else if (strcmp(mem_name, "vtan") == 0)
            {
                // Get vertex tangents array size
                size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert((got_size && num_verts == 0) || size == num_verts * 3);
                num_verts = size / 3;

                // Get vertex tangents
                this->_vertex_tangents.assign(size / 3, HalfVec3::zero());
                const auto read_size = reader.typed_array(this->_vertex_tangents.data()->vec(), size);
                assert(read_size == size);
            }
            else if (strcmp(mem_name, "vbts") == 0)
            {
                // Get vertex bitangent signs array size
                size_t size = 0;
                const bool got_size = reader.array_size(size);
                assert((got_size && num_verts == 0) || size == num_verts);
                num_verts = size;

                // Get vertex bitangent signs
                this->_bitangent_signs.assign(size, 0);
                const auto read_size = reader.typed_array(this->_bitangent_signs.data(), size);
                assert(read_size == size);
            }
            else if (strcmp(mem_name, "mtuv") == 0)
            {
                // Get uv array size
                size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert((got_size && num_verts == 0) || size == num_verts * 2);
                num_verts = size / 2;

                // Get uvs
                this->_material_uv.assign(size / 2, UHalfVec2::zero());
                const auto read_size = reader.typed_array(this->_material_uv.data()->vec(), size);
                assert(read_size == size);
            }
            else if (strcmp(mem_name, "lmuv") == 0)
            {
                // Get uv array size
                size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert((got_size && num_verts == 0) || size == num_verts * 2);
                num_verts = size / 2;

                // Get uvs
                this->_lightmap_uv.assign(size / 2, UHalfVec2::zero());
                const auto read_size = reader.typed_array(this->_lightmap_uv.data()->vec(), size);
                assert(read_size == size);
            }
            else if (strcmp(mem_name, "elem") == 0)
            {
                // Get element array size
                size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert(got_size);

                // Get elements
                this->_triangle_elements.assign(size, 0);
                const auto read_size = reader.typed_array(this->_triangle_elements.data(), size);
                assert(read_size == size);
            }
            else if (strcmp(mem_name, "mats") == 0)
            {
                // Get array size
                size_t size = 0;
                const auto got_size = reader.array_size(size);
                assert(got_size);

                // Reserve space
                this->_materials.reserve(size);

                // Deserialize materials
                reader.enumerate_array_elements([this, &reader](size_t /*i*/)
                {
                    Material mat;
                    mat.from_archive(reader);
                    this->_materials.push_back(std::move(mat));
                });
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

    size_t StaticMesh::num_verts() const
    {
        return _vertex_positions.size();
    }

    const Vec3* StaticMesh::vertex_positions() const
    {
        if (_vertex_positions.empty())
        {
            return nullptr;
        }

        return _vertex_positions.data();
    }

    const HalfVec3* StaticMesh::vertex_normals() const
    {
        if (_vertex_normals.empty())
        {
            return nullptr;
        }

        return _vertex_normals.data();
    }

    const HalfVec3* StaticMesh::vertex_tangents() const
    {
        if (_vertex_tangents.empty())
        {
            return nullptr;
        }

        return _vertex_tangents.data();
    }

    const int8_t* StaticMesh::bitangent_signs() const
    {
        if (_bitangent_signs.empty())
        {
            return nullptr;
        }

        return _bitangent_signs.data();
    }

    const UHalfVec2* StaticMesh::material_uv() const
    {
        if (_material_uv.empty())
        {
            return nullptr;
        }

        return _material_uv.data();
    }

    const UHalfVec2* StaticMesh::lightmap_uv() const
    {
        if (_lightmap_uv.empty())
        {
            return material_uv();
        }

        return _lightmap_uv.data();
    }

    size_t StaticMesh::num_triangles() const
    {
        return _triangle_elements.size() / 3;
    }

    size_t StaticMesh::num_triangle_elements() const
    {
        return _triangle_elements.size();
    }

    const uint32_t* StaticMesh::triangle_elements() const
    {
        if (_triangle_elements.empty())
        {
            return nullptr;
        }

        return _triangle_elements.data();
    }

    size_t StaticMesh::num_materials() const
    {
        return _materials.size();
    }

    const StaticMesh::Material* StaticMesh::materials() const
    {
        return _materials.data();
    }
}
