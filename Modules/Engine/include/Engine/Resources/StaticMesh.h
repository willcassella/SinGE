// StaticMesh.h
#pragma once

#include <vector>
#include "Material.h"

namespace sge
{
	struct SGE_ENGINE_API StaticMesh
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		StaticMesh();

		///////////////////
		///   Methods   ///
	public:

		void load(std::istream& file, std::size_t end);

		std::size_t num_vertices() const
		{
			return _vertex_positions.size();
		}

		std::size_t num_uv_maps() const
		{
			return 1;
		}

		const Vec3* vertex_positions() const
		{
			if (_vertex_positions.empty())
			{
				return nullptr;
			}

			return &_vertex_positions.front();
		}

		const Vec3* vertex_normals() const
		{
			if (_vertex_normals.empty())
			{
				return nullptr;
			}

			return &_vertex_normals.front();
		}

		const Vec2* uv_map_0() const
		{
			return _uv_map_0.data();
		}

		//////////////////
		///   Fields   ///
	private:

		std::vector<Vec3> _vertex_positions;
		std::vector<Vec3> _vertex_normals;
		std::vector<Vec2> _uv_map_0;
	};

	struct SGE_ENGINE_API StaticMeshAsset
	{
		//////////////////
		///   Fields   ///
	public:

		std::string mesh;
		std::string material;
	};
}
