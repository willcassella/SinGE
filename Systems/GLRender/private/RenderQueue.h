// RenderQueue.h
#pragma once

#include <vector>
#include <unordered_map>
#include <Core/Math/Mat4.h>
#include "GLMaterial.h"
#include "GLStaticMesh.h"

namespace sge
{
	struct GLMaterial;

	struct RenderQueue
	{
		//////////////////
		///   Fields   ///
	public:

		GLStaticMesh::VAO vao;
		std::vector<Mat4> matrices;
	};

	struct RenderList
	{
		//////////////////
		///   Fields   ///
	public:

		std::unordered_map<GLMaterial::Id, RenderQueue> unmodified_renderables;
	};
}
