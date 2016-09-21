// GLStaticMesh.cpp - Copyright 2013-2016 Will Cassella, All Rights Reserved

#include <fstream>
#include <Core/Containers/Array.h>
#include "glew.h"
#include "../include/GLRender/GLStaticMesh.h"

namespace willow
{
	////////////////////////
	///   Constructors   ///

	GLStaticMesh::GLStaticMesh(const StaticMesh& mesh)
	{
		this->_numVertices = static_cast<GLInteger>(mesh.vertices.Size());

		// Generate buffers and upload data
		glGenVertexArrays(1, &this->_vao);
		glBindVertexArray(this->_vao);

		glGenBuffers(1, &this->_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.Size() * sizeof(StaticMesh::Vertex), mesh.vertices.CArray(), GL_STATIC_DRAW);

		// Setup vertex specification
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StaticMesh::Vertex), nullptr); // "vPosition" attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(StaticMesh::Vertex), (void*)offsetof(StaticMesh::Vertex, uv)); // "vTexCoord" attribute
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(StaticMesh::Vertex), (void*)offsetof(StaticMesh::Vertex, normal)); // "vNormal" attribute

		glBindVertexArray(0);
	}

	GLStaticMesh::GLStaticMesh(GLStaticMesh&& move)
	{
		this->_vao = move._vao;
		this->_vbo = move._vbo;
		this->_numVertices = move._numVertices;

		move._vao = 0;
		move._vbo = 0;
		move._numVertices = 0;
	}

	GLStaticMesh::~GLStaticMesh()
	{
		glDeleteBuffers(1, &this->_vbo);
		glDeleteVertexArrays(1, &_vao);
	}

	///////////////////
	///   Methods   ///

	void GLStaticMesh::bind() const
	{
		glBindVertexArray(this->_vao);
	}
}
