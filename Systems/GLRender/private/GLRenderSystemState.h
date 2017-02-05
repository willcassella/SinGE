// GLRenderSystemState.h
#pragma once

#include "../include/GLRender/GLRenderSystem.h"
#include "GLShader.h"
#include "GLMaterial.h"
#include "GLStaticMesh.h"
#include "GLTexture2D.h"
#include "DebugLine.h"
#include "RenderScene.h"

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

			GLShader::Id get_shader_resource(const std::string& path);

			GLMaterial::Id get_material_resource(const std::string& path);

			GLStaticMesh::VAO get_static_mesh_resource(const std::string& path);

			GLTexture2D::Id get_texture_2d_resource(const std::string& path);

			//////////////////
			///   Fields   ///
		public:

			// Config
			GLint width;
			GLint height;
			GLStaticMesh::VAO missing_mesh;
			GLMaterial::Id missing_material;

			// Resources
			std::unordered_map<std::string, GLShader::Id> shader_resources;
			std::unordered_map<std::string, GLMaterial::Id> material_resources;
			std::unordered_map<std::string, GLStaticMesh::VAO> static_mesh_resources;
			std::unordered_map<std::string, GLTexture2D::Id> texture_2d_resources;

            std::map<GLShader::Id, GLShader> shaders;
            std::map<GLMaterial::Id, GLMaterial> materials;
            std::map<GLStaticMesh::VAO, GLStaticMesh> static_meshes;
            std::map<GLTexture2D::Id, GLTexture2D> texture_2ds;

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

            // Scene data
            bool initialized_render_scene = false;
            RenderScene render_scene;
		};
	}
}
