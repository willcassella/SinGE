// StaticMesh.h
#pragma once

#include <vector>
#include <Core/Math/Vec3.h>
#include "../config.h"

namespace sge
{
	struct SGE_ENGINE_API StaticMesh
	{
		///////////////////
		///   Methods   ///
	public:

		std::size_t get_num_vertices() const
		{
			return _vertex_positions.size();
		}

		const Vec3* get_vertex_positions() const
		{
			if (_vertex_positions.empty())
			{
				return nullptr;
			}

			return &_vertex_positions.front();
		}

		const Vec3* get_vertex_normals() const
		{
			if (_vertex_normals.empty())
			{
				return nullptr;
			}

			return &_vertex_normals.front();
		}

		////////////////
		///   Data   ///
	private:

		std::vector<Vec3> _vertex_positions;
		std::vector<Vec3> _vertex_normals;
		std::vector<Vec2> _uv_map_1;
		std::vector<Vec2> _uv_map_2;
		std::vector<Vec2> _uv_map_3;
	};
}
