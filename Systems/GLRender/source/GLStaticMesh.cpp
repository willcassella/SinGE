// GLStaticMesh.cpp

#include "../private/GLStaticMesh.h"
#include "../private/GLMaterial.h"

namespace sge
{
	namespace gl_render
	{
        template <typename CountGetterT, typename DataGetterT>
        void buffer_static_data(
            GLenum target,
            GLuint buffer,
            std::size_t num_total_elems,
            std::size_t elem_size,
            const StaticMesh::SubMesh* sub_meshes,
            std::size_t num_sub_meshes,
            CountGetterT count_getter,
            DataGetterT data_getter)
        {
            // Create buffer
            glBindBuffer(target, buffer);
            glBufferData(target, num_total_elems * elem_size, nullptr, GL_STATIC_DRAW);

            // Upload data for each mesh
            GLintptr offset = 0;
            for (std::size_t i = 0; i < num_sub_meshes; ++i)
            {
                const GLsizeiptr size = (sub_meshes[i].*count_getter)() * elem_size;
                glBufferSubData(target, offset, size, (sub_meshes[i].*data_getter)());
                offset += size;
            }
        }

		GLStaticMesh::GLStaticMesh(const StaticMesh& mesh)
            : _num_elements(0)
	    {
            const auto num_sub_meshes = mesh.num_sub_meshes();
            const auto* sub_meshes = mesh.sub_meshes();

		    // Count up all vertices and elements
            std::size_t num_verts = 0;
            for (std::size_t i = 0; i < num_sub_meshes; ++i)
            {
                num_verts += sub_meshes[i].num_verts();
                _num_elements += static_cast<GLint>(sub_meshes[i].num_triangles() * 3);
            }

			// Set up VAO
			glGenVertexArrays(1, &_vao);
			glBindVertexArray(_vao);

			// Create buffers
			glGenBuffers(NUM_BUFFERS, _buffers.data());

            // Upload element data
            buffer_static_data(
                GL_ELEMENT_ARRAY_BUFFER,
                element_buffer(),
                _num_elements,
                sizeof(uint32),
                sub_meshes,
                num_sub_meshes,
                &StaticMesh::SubMesh::num_triangle_elements,
                &StaticMesh::SubMesh::triangle_elements);

            // Upload vertex position data
            buffer_static_data(
                GL_ARRAY_BUFFER,
                position_buffer(),
                num_verts,
                sizeof(Vec3),
                sub_meshes,
                num_sub_meshes,
                &StaticMesh::SubMesh::num_verts,
                &StaticMesh::SubMesh::vertex_positions);

			// Define vertex position specification
			glEnableVertexAttribArray(GLMaterial::POSITION_ATTRIB_LOCATION);
			glVertexAttribPointer(GLMaterial::POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), nullptr);

			// Upload normal buffer
            buffer_static_data(
                GL_ARRAY_BUFFER,
                normal_buffer(),
                num_verts,
                sizeof(HalfVec3),
                sub_meshes,
                num_sub_meshes,
                &StaticMesh::SubMesh::num_verts,
                &StaticMesh::SubMesh::vertex_normals);

			// Define vertex normal specification
			glEnableVertexAttribArray(GLMaterial::NORMAL_ATTRIB_LOCATION);
			glVertexAttribPointer(GLMaterial::NORMAL_ATTRIB_LOCATION, 3, GL_SHORT, GL_TRUE, sizeof(HalfVec3), nullptr);

            // Upload tangent buffer
            buffer_static_data(
                GL_ARRAY_BUFFER,
                tangent_buffer(),
                num_verts,
                sizeof(HalfVec3),
                sub_meshes,
                num_sub_meshes,
                &StaticMesh::SubMesh::num_verts,
                &StaticMesh::SubMesh::vertex_tangents);

            // Define vertex tangent specification
            glEnableVertexAttribArray(GLMaterial::TANGENT_ATTRIB_LOCATION);
            glVertexAttribPointer(GLMaterial::TANGENT_ATTRIB_LOCATION, 3, GL_SHORT, GL_TRUE, sizeof(HalfVec3), nullptr);

            // Upload bitangent sign buffer
            buffer_static_data(
                GL_ARRAY_BUFFER,
                bitangent_sign_buffer(),
                num_verts,
                1,
                sub_meshes,
                num_sub_meshes,
                &StaticMesh::SubMesh::num_verts,
                &StaticMesh::SubMesh::bitangent_signs);

            // Define bitangent specification
            glEnableVertexAttribArray(GLMaterial::BITANGENT_SIGN_ATTRIB_LOCATION);
            glVertexAttribPointer(GLMaterial::BITANGENT_SIGN_ATTRIB_LOCATION, 1, GL_BYTE, GL_FALSE, 1, nullptr);

			// Upload uv data
            buffer_static_data(
                GL_ARRAY_BUFFER,
                uv_buffer(),
                num_verts,
                sizeof(HalfVec2),
                sub_meshes,
                num_sub_meshes,
                &StaticMesh::SubMesh::num_verts,
                &StaticMesh::SubMesh::material_uv);

			// Define vertex uv specification
			glEnableVertexAttribArray(GLMaterial::TEXCOORD_ATTRIB_LOCATION);
			glVertexAttribPointer(GLMaterial::TEXCOORD_ATTRIB_LOCATION, 2, GL_SHORT, GL_TRUE, sizeof(HalfVec2), nullptr);

            // Unbind
			glBindVertexArray(0);
		}

		GLStaticMesh::GLStaticMesh(GLStaticMesh&& move)
			: _vao(move._vao),
			_num_elements(move._num_elements),
			_buffers(move._buffers)
		{
			move._vao = 0;
			move._buffers.fill(0);
			move._num_elements = 0;
		}

		GLStaticMesh::~GLStaticMesh()
		{
			glDeleteBuffers(NUM_BUFFERS, _buffers.data());
			glDeleteVertexArrays(1, &_vao);
		}

		void GLStaticMesh::bind() const
		{
			glBindVertexArray(_vao);
		}
	}
}
