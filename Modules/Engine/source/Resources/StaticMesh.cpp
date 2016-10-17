// StaticMesh.cpp

#include <fstream>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../../include/Engine/Resources/StaticMesh.h"

SGE_REFLECT_TYPE(sge::StaticMesh)
.flags(TF_SCRIPT_NOCONSTRUCT);

namespace sge
{
	struct OldVertex
	{
		Vec3 position;
		Vec2 uv;
		Vec3 normal;
	};

	StaticMesh::StaticMesh()
	{
	}

	void StaticMesh::load(const std::string& path)
	{
		auto file = fopen(path.c_str(), "rb");

		// Get the number of vertices
		uint32 numVerts = 0;
		fread(&numVerts, sizeof(uint32), 1, file);

		// Read in vertices
		auto* buff = (OldVertex*)calloc(sizeof(OldVertex), numVerts);
		fread(buff, sizeof(OldVertex), numVerts, file);

		// Reserve space for vertices
		_vertex_positions.reserve(numVerts);
		_vertex_normals.reserve(numVerts);
		_uv_map_0.reserve(numVerts);

		// Convert vertices to new format
		for (uint32 i = 0; i < numVerts; ++i)
		{
			_vertex_positions.push_back(buff[i].position);
			_vertex_normals.push_back(buff[i].normal);
			_uv_map_0.push_back(buff[i].uv);
		}

		free(buff);
		fclose(file);
	}
}
