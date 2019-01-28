// StaticMesh.h
#pragma once

#include <vector>
#include <Core/Math/IVec2.h>
#include <Core/Math/IVec3.h>
#include <Core/Containers/FixedString.h>
#include "Material.h"

namespace sge
{
    struct SGE_RESOURCE_API StaticMesh
    {
        SGE_REFLECTED_TYPE;

        struct SGE_RESOURCE_API Material
        {
            ///////////////////
            ///   Methods   ///
        public:

            void to_archive(ArchiveWriter& writer) const;

            void from_archive(ArchiveReader& reader);

            const std::string& path() const;

            uint32 start_elem_index() const;

            uint32 num_elem_indices() const;

            //////////////////
            ///   Fields   ///
        private:

            std::string _path;
            uint32 _start_elem_index = 0;
            uint32 _num_elem_indices = 0;
        };

        ////////////////////////
        ///   Constructors   ///
    public:

        StaticMesh();

        ///////////////////
        ///   Methods   ///
    public:

        void to_archive(ArchiveWriter& writer) const;

        void from_archive(ArchiveReader& reader);

        bool from_file(const char* path);

        std::size_t num_verts() const;

        const Vec3* vertex_positions() const;

        const HalfVec3* vertex_normals() const;

        const HalfVec3* vertex_tangents() const;

        const int8* bitangent_signs() const;

        const UHalfVec2* material_uv() const;

        const UHalfVec2* lightmap_uv() const;

        std::size_t num_triangles() const;

        std::size_t num_triangle_elements() const;

        const uint32* triangle_elements() const;

        std::size_t num_materials() const;

        const Material* materials() const;

        //////////////////
        ///   Fields   ///
    private:

        std::vector<Vec3> _vertex_positions;
        std::vector<HalfVec3> _vertex_normals;
        std::vector<HalfVec3> _vertex_tangents;
        std::vector<int8> _bitangent_signs;
        std::vector<UHalfVec2> _material_uv;
        std::vector<UHalfVec2> _lightmap_uv;
        std::vector<uint32> _triangle_elements;
        std::vector<Material> _materials;
    };
}
