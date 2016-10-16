// GLStaticMesh.h
#pragma once

#include <array>
#include <Engine/Resources/StaticMesh.h>
#include "glew.h"

namespace sge
{
	struct GLStaticMesh
	{
		static constexpr GLsizei NUM_BUFFERS = 3;
		using VAO = GLuint;

		////////////////////////
		///   Constructors   ///
	public:

		/** Uploads the given Mesh asset to the GPU. */
		GLStaticMesh(const StaticMesh& mesh);

		GLStaticMesh(GLStaticMesh&& move);
		~GLStaticMesh();

		///////////////////
		///   Methods   ///
	public:

		void bind() const;

		VAO vao() const
		{
			return _vao;
		}

		GLuint position_buffer() const
		{
			return _buffers[0];
		}

		GLuint normal_buffer() const
		{
			return _buffers[1];
		}

		GLuint uv_buffer() const
		{
			return _buffers[2];
		}

		GLint num_vertices() const
		{
			return _num_vertices;
		}

		//////////////////
		///   Fields   ///
	private:

		VAO _vao;
		GLint _num_vertices;
		std::array<GLuint, NUM_BUFFERS> _buffers;
	};
}
