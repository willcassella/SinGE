// GLRenderSystemState.h
#pragma once

#include "../include/GLRender/GLRenderSystem.h"
#include "GLShader.h"
#include "GLMaterial.h"
#include "GLStaticMesh.h"
#include "GLTexture2D.h"
#include "DebugLine.h"

namespace sge
{
	namespace gl_render
	{
		namespace GBufferLayer
		{
			enum : GLsizei
			{
				DEPTH_STENCIL,
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

			// Config
			GLint width;
			GLint height;
			StaticMesh missing_mesh;
			Material missing_material;

			// Resources
			std::unordered_map<std::string, GLShader> shaders;
			std::unordered_map<std::string, GLMaterial> materials;
			std::unordered_map<std::string, GLStaticMesh> static_meshes;
			std::unordered_map<std::string, GLTexture2D> textures;

			// The default framebuffer
			GLint default_framebuffer;

			// GBuffer stuff
			GLuint gbuffer_framebuffer;
            GLuint post_buffer;
			std::array<GLuint, GBufferLayer::NUM_LAYERS> gbuffer_layers;

			// Sprite quad
			GLuint sprite_vao;
			GLuint sprite_vbo;

			// Screen quad
			GLuint screen_quad_program;

            // Debug draw line buffer
            GLuint debug_line_vao;
            GLuint debug_line_vbo;

            // Debug line shader program
            GLuint debug_line_program;
            GLint debug_line_view_uniform;
            GLint debug_line_proj_uniform;

            // Debug line buffer
            std::vector<DebugLineVert> frame_debug_lines;
		};
	}
}
