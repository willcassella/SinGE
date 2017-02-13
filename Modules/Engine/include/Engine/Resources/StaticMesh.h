// StaticMesh.h
#pragma once

#include <vector>
#include <Core/Math/HalfVec2.h>
#include <Core/Math/HalfVec3.h>
#include "Material.h"

namespace sge
{
	struct SGE_ENGINE_API StaticMesh
	{
		SGE_REFLECTED_TYPE;

        struct SGE_ENGINE_API SubMesh
        {
            ///////////////////
            ///   Methods   ///
        public:

            void serialize(ArchiveWriter& writer) const;

            void deserialize(ArchiveReader& reader, std::string name);

            ///////////////////
            ///   Methods   ///
        public:

            const std::string& name() const;

            const std::string& default_material() const;

            std::size_t num_verts() const;

            const Vec3* vertex_positions() const;

            const HalfVec3* vertex_normals() const;

            const HalfVec3* vertex_tangents() const;

            const int8* bitangent_signs() const;

            const HalfVec2* material_uv() const;

            std::size_t num_triangles() const;

            std::size_t num_triangle_elements() const;

            const uint32* triangle_elements() const;

            //////////////////
            ///   Fields   ///
        private:

            std::string _name;
            std::string _default_material;
            std::vector<Vec3> _vertex_positions;
            std::vector<HalfVec3> _vertex_normals;
            std::vector<HalfVec3> _vertex_tangents;
            std::vector<int8> _bitangent_signs;
            std::vector<HalfVec2> _material_uv;
            std::vector<uint32> _triangle_elements;
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

        std::size_t num_sub_meshes() const;

        const SubMesh* sub_meshes() const;

		//////////////////
		///   Fields   ///
	private:

        std::vector<SubMesh> _sub_meshes;
	};
}
