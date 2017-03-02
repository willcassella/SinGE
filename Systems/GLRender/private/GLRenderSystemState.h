// GLRenderSystemState.h
#pragma once

#include "../include/GLRender/GLRenderSystem.h"
#include "GLShader.h"
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
				POSITION,
				NORMAL,
				ALBEDO,
				ROUGHNESS_METALLIC,
                IRRADIANCE,
				NUM_LAYERS
			};
		}

        /* These constants specify the attachment index for buffer layers. */
        static constexpr GLenum GBUFFER_DEPTH_STENCIL_ATTACHMENT = GL_DEPTH_STENCIL_ATTACHMENT;
        static constexpr GLenum GBUFFER_POSITION_ATTACHMENT = GL_COLOR_ATTACHMENT0;
        static constexpr GLenum GBUFFER_NORMAL_ATTACHMENT = GL_COLOR_ATTACHMENT1;
        static constexpr GLenum GBUFFER_ALBEDO_ATTACHMENT = GL_COLOR_ATTACHMENT2;
        static constexpr GLenum GBUFFER_ROUGHNESS_METALLIC_ATTACHMENT = GL_COLOR_ATTACHMENT3;
        static constexpr GLenum GBUFFER_IRRADIANCE_ATTACHMENT = GL_COLOR_ATTACHMENT4;
        static constexpr GLenum POST_BUFFER_HDR_ATTACHMENT = GL_COLOR_ATTACHMENT0;

        /* These constants define the internal format for the gbuffer layers. */
        static constexpr GLenum GBUFFER_DEPTH_STENCIL_INTERNAL_FORMAT = GL_DEPTH24_STENCIL8;
        static constexpr GLenum GBUFFER_POSITION_INTERNAL_FORMAT = GL_RGB32F;
        static constexpr GLenum GBUFFER_NORMAL_INTERNAL_FORMAT = GL_RGB32F;
        static constexpr GLenum GBUFFER_ALBEDO_INTERNAL_FORMAT = GL_RGB8;
        static constexpr GLenum GBUFFER_ROUGHNESS_METALLIC_INTERNAL_FORMAT = GL_RG16F;
        static constexpr GLenum GBUFFER_IRRADIANCE_INTERNAL_FORMAT = GL_RGBA32F;
        static constexpr GLenum POST_BUFFER_HDR_INTERNAL_FORMAT = GL_RGB32F;

        /* These constants are used when initializing or resizing gbuffer layers.
        * They're pretty much meaningless, but are used to ensure consistency. */
        static constexpr GLenum GBUFFER_DEPTH_STENCIL_UPLOAD_FORMAT = GL_DEPTH_STENCIL;
        static constexpr GLenum GBUFFER_DEPTH_STENCIL_UPLOAD_TYPE = GL_UNSIGNED_INT_24_8;
        static constexpr GLenum GBUFFER_POSITION_UPLOAD_FORMAT = GL_RGB;
        static constexpr GLenum GBUFFER_POSITION_UPLOAD_TYPE = GL_FLOAT;
        static constexpr GLenum GBUFFER_NORMAL_UPLOAD_FORMAT = GL_RGB;
        static constexpr GLenum GBUFFER_NORMAL_UPLOAD_TYPE = GL_FLOAT;
        static constexpr GLenum GBUFFER_ALBEDO_UPLOAD_FORMAT = GL_RGB;
        static constexpr GLenum GBUFFER_ALBEDO_UPLOAD_TYPE = GL_UNSIGNED_BYTE;
        static constexpr GLenum GBUFFER_ROUGHNESS_METALLIC_UPLOAD_FORMAT = GL_RG;
        static constexpr GLenum GBUFFER_ROUGHNESS_METALLIC_UPLOAD_TYPE = GL_UNSIGNED_BYTE;
        static constexpr GLenum GBUFFER_IRRADIANCE_UPLOAD_FORMAT = GL_RGBA;
        static constexpr GLenum GBUFFER_IRRADIANCE_UPLOAD_TYPE = GL_FLOAT;
        static constexpr GLenum POST_BUFFER_HDR_UPLOAD_FORMAT = GL_RGB;
        static constexpr GLenum POST_BUFFER_HDR_UPLOAD_TYPE = GL_FLOAT;

		struct GLRenderSystem::State
		{
			///////////////////
			///   Methods   ///
		public:

			const gl_material::Material& get_material_resource(const std::string& path);

			const gl_static_mesh::StaticMesh& get_static_mesh_resource(const std::string& path);

			GLShader::Id get_shader_resource(const std::string& path);

			GLuint get_texture_2d_resource(const std::string& path, bool hdr);

			//////////////////
			///   Fields   ///
		public:

			// Config
			GLint width;
			GLint height;
			gl_static_mesh::StaticMesh missing_mesh;
            gl_material::Material missing_material;

			// Resources
			std::unordered_map<std::string, GLShader> shader_resources;
			std::unordered_map<std::string, gl_material::Material> material_resources;
			std::unordered_map<std::string, gl_static_mesh::StaticMesh> static_mesh_resources;
			std::unordered_map<std::string, GLuint> texture_2d_resources;

			// The default framebuffer
			GLint default_framebuffer;

			// Gbuffer
			GLuint gbuffer_framebuffer;
			std::array<GLuint, GBufferLayer::NUM_LAYERS> gbuffer_layers;

            // Post-processing framebuffer
            GLuint post_framebuffer;
		    GLuint post_buffer_hdr;

			// Sprite quad
			GLuint sprite_vao;
			GLuint sprite_vbo;

			// Screen quad
			GLuint scene_shader_program;
            GLuint post_shader_program;
            GLint scene_program_view_uniform;

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
