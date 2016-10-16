// GLStaticMesh.cpp

#include "../private/GLStaticMesh.h"
#include "../private/GLMaterial.h"

namespace sge
{
	GLStaticMesh::GLStaticMesh(const StaticMesh& mesh)
	{
		_num_vertices = static_cast<GLint>(mesh.num_vertices());

		// Set up VAO
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// Create buffers
		glGenBuffers(NUM_BUFFERS, _buffers.data());

		// Upload vertex position data
		glBindBuffer(GL_ARRAY_BUFFER, this->position_buffer());
		glBufferData(GL_ARRAY_BUFFER, _num_vertices * sizeof(Vec3), mesh.vertex_positions(), GL_STATIC_DRAW);

		// Define vertex position specification
		glEnableVertexAttribArray(GLMaterial::POSITION_ATTRIB_LOCATION);
		glVertexAttribPointer(GLMaterial::POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), nullptr);

		// Upload normal buffer
		//glBindBuffer(GL_ARRAY_BUFFER, this->normal_buffer());
		//glBufferData(GL_ARRAY_BUFFER, _num_vertices * sizeof(Vec3), mesh.vertex_positions(), GL_STATIC_DRAW);

		// Define vertex normal specification
		//glEnableVertexAttribArray(GLMaterial::NORMAL_ATTRIB_LOCATION);
		//glVertexAttribPointer(GLMaterial::NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), nullptr);

		// Upload uv data
		//glBindBuffer(GL_ARRAY_BUFFER, this->uv_buffer());
		//glBufferData(GL_ARRAY_BUFFER, _num_vertices * sizeof(Vec2), mesh.uv_map_0(), GL_STATIC_DRAW);

		// Define vertex uv specification
		//glEnableVertexAttribArray(GLMaterial::TEXCOORD_ATTRIB_LOCATION);
		//glVertexAttribPointer(GLMaterial::TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), nullptr);

		glBindVertexArray(0);
	}

	GLStaticMesh::GLStaticMesh(GLStaticMesh&& move)
		: _vao(move._vao),
		_num_vertices(move._num_vertices),
		_buffers(move._buffers)
	{
		move._vao = 0;
		move._buffers.fill(0);
		move._num_vertices = 0;
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
