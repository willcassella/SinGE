// StaticMesh.cpp

#include <fstream>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../../include/Engine/Resources/StaticMesh.h"

SGE_REFLECT_TYPE(sge::StaticMesh)
.flags(TF_SCRIPT_NOCONSTRUCT);

namespace sge
{
	StaticMesh::StaticMesh()
	{
	}

	void StaticMesh::to_archive(ArchiveWriter& writer) const
	{
		// Write all vertex positions
		writer.push_object_member("vertex_positions");
		writer.typed_array(_vertex_positions.data()->vec(), _vertex_positions.size() * 3);
		writer.pop();

		// Write all vertex normals
		writer.push_object_member("vertex_normals");
		writer.typed_array(_vertex_normals.data()->vec(), _vertex_normals.size() * 3);
		writer.pop();

		// Write the first UV layer
		writer.push_object_member("uv0");
		writer.typed_array(_uv_map_0.data()->vec(), _uv_map_0.size() * 2);
		writer.pop();
	}

	void StaticMesh::from_file(const std::string& path)
	{
		auto file = fopen(path.c_str(), "rb");

		// Get the number of vertex positions
		uint32 num = 0;
		fread(&num, sizeof(num), 1, file);

		// Read in vertex positions
		_vertex_positions.assign(num, Vec3::zero());
		fread(_vertex_positions.data(), sizeof(Vec3), num, file);

		// Get the number of vertex normals
		fread(&num, sizeof(num), 1, file);

		// Read in vertex normals
		_vertex_normals.assign(num, Vec3::zero());
		fread(_vertex_normals.data(), sizeof(Vec3), num, file);

		// Get the number of uv coordinates
		fread(&num, sizeof(num), 1, file);
		_uv_map_0.assign(num, Vec2::zero());

		// Read in first UV map
		fread(_uv_map_0.data(), sizeof(Vec2), num, file);

		fclose(file);
	}
}
