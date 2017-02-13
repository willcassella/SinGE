// GLStaticMesh.h
#pragma once

#include <array>
#include <Engine/Resources/StaticMesh.h>
#include "glew.h"

namespace sge
{
	namespace gl_render
	{
		struct GLStaticMesh
		{
			static constexpr GLsizei NUM_BUFFERS = 6;
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

            GLuint element_buffer() const
			{
                return _buffers[0];
			}

			GLuint position_buffer() const
			{
				return _buffers[1];
			}

			GLuint normal_buffer() const
			{
				return _buffers[2];
			}

            GLuint tangent_buffer() const
			{
                return _buffers[3];
			}

            GLuint bitangent_sign_buffer() const
			{
                return _buffers[4];
			}

			GLuint uv_buffer() const
			{
				return _buffers[5];
			}

			GLint num_element() const
			{
				return _num_elements;
			}

			//////////////////
			///   Fields   ///
		private:

			VAO _vao;
			GLint _num_elements;
			std::array<GLuint, NUM_BUFFERS> _buffers;
		};
	}
}
