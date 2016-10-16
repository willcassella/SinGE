// GLRenderSystemState.h
#pragma once

#include "../include/GLRender/GLRenderSystem.h"
#include "GLShader.h"
#include "GLMaterial.h"
#include "GLStaticMesh.h"
#include "GLTexture2D.h"

namespace sge
{
	namespace GBufferLayer
	{
		enum: GLsizei
		{
			DEPTH,
			NORMAL,
			POSITION,
			DIFFUSE,
			SPECULAR,
			NUM_LAYERS
		};
	}

	struct GLRenderSystem::State
	{
		///////////////////
		///   Methods   ///
	public:

		const GLShader& find_shader(const std::string& path);

		const GLMaterial& find_material(const std::string& path);

		const GLStaticMesh& find_static_mesh(const std::string& path);

		const GLTexture2D& find_texture_2d(const std::string& path);

		//////////////////
		///   Fields   ///
	public:

		std::unordered_map<std::string, GLShader> shaders;
		std::unordered_map<std::string, GLMaterial> materials;
		std::unordered_map<std::string, GLStaticMesh> static_meshes;
		std::unordered_map<std::string, GLTexture2D> textures;

		// The default framebuffer
		GLint default_framebuffer;

		// GBuffer stuff
		GLuint gbuffer_framebuffer;
		std::array<GLuint, GBufferLayer::NUM_LAYERS> gbuffer_layers;

		// Screen data
		GLsizei width;
		GLsizei height;

		// Sprite quad
		GLuint sprite_vao;
		GLuint sprite_vbo;

		// Screen quad
		GLuint screen_quad_program;
	};
}
