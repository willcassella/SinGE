// GLRenderSystem.cpp

#include <iostream>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Resource/Archives/JsonArchive.h>
#include <Engine/Components/CTransform3D.h>
#include <Engine/Components/Display/CStaticMesh.h>
#include <Engine/Components/Display/CCamera.h>
#include <Engine/Components/Display/CLightMaskReceiver.h>
#include <Engine/Components/Display/CLightMaskObstructor.h>
#include <Engine/Tags/FDebugDraw.h>
#include <Engine/Scene.h>
#include <Engine/SystemFrame.h>
#include <Engine/UpdatePipeline.h>
#include "../include/GLRender/Config.h"
#include "../private/GLRenderSystemState.h"
#include "../private/DebugLine.h"

SGE_REFLECT_TYPE(sge::gl_render::GLRenderSystem);

namespace sge
{
	namespace gl_render
	{
        static void set_render_target_params()
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        static void upload_render_target_data(
            GLsizei width,
            GLsizei height,
            GLenum internal_format,
            GLenum upload_format,
            GLenum upload_type)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, upload_format, upload_type, nullptr);
        }

		static void create_gbuffer_layer(
			GLuint layer,
			GLenum attachment,
			GLsizei width,
			GLsizei height,
			GLenum internal_format,
			GLenum upload_format,
			GLenum upload_type)
		{
			glBindTexture(GL_TEXTURE_2D, layer);
            set_render_target_params();
            upload_render_target_data(width, height, internal_format, upload_format, upload_type);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, layer, 0);
		}

        /* These constants specify the attachment index for buffer layers. */
		static constexpr GLenum GBUFFER_DEPTH_STENCIL_ATTACHMENT = GL_DEPTH_STENCIL_ATTACHMENT;
		static constexpr GLenum GBUFFER_POSITION_ATTACHMENT = GL_COLOR_ATTACHMENT0;
		static constexpr GLenum GBUFFER_NORMAL_ATTACHMENT = GL_COLOR_ATTACHMENT1;
		static constexpr GLenum GBUFFER_DIFFUSE_ATTACHMENT = GL_COLOR_ATTACHMENT2;
		static constexpr GLenum GBUFFER_SPECULAR_ATTACHMENT = GL_COLOR_ATTACHMENT3;

        /* These constants define the internal format for the gbuffer layers. */
        static constexpr GLenum GBUFFER_DEPTH_STENCIL_INTERNAL_FORMAT = GL_DEPTH24_STENCIL8;
        static constexpr GLenum GBUFFER_POSITION_INTERNAL_FORMAT = GL_RGB32F;
        static constexpr GLenum GBUFFER_NORMAL_INTERNAL_FORMAT = GL_RGB32F;
        static constexpr GLenum GBUFFER_DIFFUSE_INTERNAL_FORMAT = GL_RGB8;
        static constexpr GLenum GBUFFER_SPECULAR_INTERNAL_FORMAT = GL_R8;
        static constexpr GLenum POST_BUFFER_INTERNAL_FORMAT = GL_RGB32F;

        /* These constants are used when initializing or resizing gbuffer layers.
         * They're pretty much meaningless, but are used to ensure consistency. */
	    static constexpr GLenum GBUFFER_DEPTH_STENCIL_UPLOAD_FORMAT = GL_DEPTH_STENCIL;
        static constexpr GLenum GBUFFER_DEPTH_STENCIL_UPLOAD_TYPE = GL_UNSIGNED_INT_24_8;
        static constexpr GLenum GBUFFER_POSITION_UPLOAD_FORMAT = GL_RGB;
        static constexpr GLenum GBUFFER_POSITION_UPLOAD_TYPE = GL_FLOAT;
        static constexpr GLenum GBUFFER_NORMAL_UPLOAD_FORMAT = GL_RGB;
        static constexpr GLenum GBUFFER_NORMAL_UPLOAD_TYPE = GL_FLOAT;
        static constexpr GLenum GBUFFER_DIFFUSE_UPLOAD_FORMAT = GL_RGB;
        static constexpr GLenum GBUFFER_DIFFUSE_UPLOAD_TYPE = GL_UNSIGNED_BYTE;
        static constexpr GLenum GBUFFER_SPECULAR_UPLOAD_FORMAT = GL_RED;
        static constexpr GLenum GBUFFER_SPECULAR_UPLOAD_TYPE = GL_UNSIGNED_BYTE;
        static constexpr GLenum POST_BUFFER_UPLOAD_FORMAT = GL_RGB;
        static constexpr GLenum POST_BUFFER_UPLOAD_TYPE = GL_FLOAT;

		////////////////////////
		///   Constructors   ///

		GLRenderSystem::GLRenderSystem(const Config& config)
		{
			assert(config.validate() /*The given GLRenderSystem config object is not valid*/);

			// Create the internal state object
			_state = std::make_unique<GLRenderSystem::State>();
			_state->width = config.viewport_width;
			_state->height = config.viewport_height;

			// Create the default resources for the internal state object
			_state->missing_mesh.from_file(config.missing_mesh.c_str());

			// Create the default material resource
			{
				JsonArchive missing_material_archive;
				missing_material_archive.from_file(config.missing_material.c_str());
				missing_material_archive.deserialize_root(_state->missing_material);
			}

			// Initialize GLEW
			glewExperimental = GL_TRUE;
			glewInit();
			glGetError(); // Sometimes GLEW initialization generates an error, pop it off the stack.

			// Initialize OpenGL
            glLineWidth(1);
			glClearColor(0, 0, 0, 1);
			glClearDepth(1.f);
			glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            glEnable(GL_FRAMEBUFFER_SRGB);

			// Get the default framebuffer
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_state->default_framebuffer);

			// Create a framebuffer for deferred rendering (GBuffer)
			glGenFramebuffers(1, &_state->gbuffer_framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, _state->gbuffer_framebuffer);
			glGenTextures(GBufferLayer::NUM_LAYERS, _state->gbuffer_layers.data());

			// Create gbuffer depth layer
			create_gbuffer_layer(
				_state->gbuffer_layers[GBufferLayer::DEPTH_STENCIL],
				GBUFFER_DEPTH_STENCIL_ATTACHMENT,
				_state->width,
				_state->height,
				GBUFFER_DEPTH_STENCIL_INTERNAL_FORMAT,
				GBUFFER_DEPTH_STENCIL_UPLOAD_FORMAT,
				GBUFFER_DEPTH_STENCIL_UPLOAD_TYPE);

			// Create gbuffer position layer
			create_gbuffer_layer(
				_state->gbuffer_layers[GBufferLayer::POSITION],
				GBUFFER_POSITION_ATTACHMENT,
				_state->width,
				_state->height,
				GBUFFER_POSITION_INTERNAL_FORMAT,
				GBUFFER_POSITION_UPLOAD_FORMAT,
				GBUFFER_POSITION_UPLOAD_TYPE);

			// Create gbuffer normal layer
			create_gbuffer_layer(
				_state->gbuffer_layers[GBufferLayer::NORMAL],
				GBUFFER_NORMAL_ATTACHMENT,
				_state->width,
				_state->height,
				GBUFFER_NORMAL_INTERNAL_FORMAT,
				GBUFFER_NORMAL_UPLOAD_FORMAT,
				GBUFFER_NORMAL_UPLOAD_TYPE);

			// Create gbuffer diffuse layer
			create_gbuffer_layer(
				_state->gbuffer_layers[GBufferLayer::DIFFUSE],
				GBUFFER_DIFFUSE_ATTACHMENT,
				_state->width,
				_state->height,
				GBUFFER_DIFFUSE_INTERNAL_FORMAT,
				GBUFFER_DIFFUSE_UPLOAD_FORMAT,
				GBUFFER_DIFFUSE_UPLOAD_TYPE);

			// Create gbuffer specular layer
			create_gbuffer_layer(
				_state->gbuffer_layers[GBufferLayer::SPECULAR],
				GBUFFER_SPECULAR_ATTACHMENT,
				_state->width,
				_state->height,
				GBUFFER_SPECULAR_INTERNAL_FORMAT,
				GBUFFER_SPECULAR_UPLOAD_FORMAT,
				GBUFFER_SPECULAR_UPLOAD_TYPE);

			// Make sure the GBuffer was constructed successfully
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cerr << "GLRenderSystem: Error creating GBuffer." << std::endl;
			}

            // Generate the post buffer
            glGenTextures(1, &_state->post_buffer);
            glBindTexture(GL_TEXTURE_2D, _state->post_buffer);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                POST_BUFFER_INTERNAL_FORMAT,
                _state->width,
                _state->height,
                0,
                POST_BUFFER_UPLOAD_FORMAT,
                POST_BUFFER_UPLOAD_TYPE,
                nullptr);

			constexpr float QUAD_VERTEX_DATA[] = {
				-1.f, 1.f, 0.f, 1.f,	// Top-left point
				-1.f, -1.f, 0.f, 0.f,	// Bottom-left point
				1.f, -1.f, 1.f, 0.f,	// Bottom-right point
				1.f, 1.f, 1.f, 1.f, 	// Top-right point
			};

			// Create a VAO for sprite
			glGenVertexArrays(1, &_state->sprite_vao);
			glBindVertexArray(_state->sprite_vao);

			// Create a VBO for the sprite quad and upload data
			glGenBuffers(1, &_state->sprite_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, _state->sprite_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTEX_DATA), QUAD_VERTEX_DATA, GL_STATIC_DRAW);

			// Sprite vertex specification
			glEnableVertexAttribArray(GLMaterial::POSITION_ATTRIB_LOCATION);
			glEnableVertexAttribArray(GLMaterial::TEXCOORD_ATTRIB_LOCATION);
			glVertexAttribPointer(GLMaterial::POSITION_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
			glVertexAttribPointer(GLMaterial::TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

			// Create and upload a shader program for the screen quad
			GLShader screenVShader{ GL_VERTEX_SHADER, config.viewport_vert_shader };
			GLShader screenFShader{ GL_FRAGMENT_SHADER, config.viewport_frag_shader };
			_state->screen_quad_program = glCreateProgram();
			glAttachShader(_state->screen_quad_program, screenVShader.id());
			glAttachShader(_state->screen_quad_program, screenFShader.id());

			// Bind vertex attributes
			glBindAttribLocation(_state->screen_quad_program, GLMaterial::POSITION_ATTRIB_LOCATION, GLMaterial::POSITION_ATTRIB_NAME);
			glBindAttribLocation(_state->screen_quad_program, GLMaterial::TEXCOORD_ATTRIB_LOCATION, GLMaterial::TEXCOORD_ATTRIB_NAME);

			// Link the program and detach shaders
			glLinkProgram(_state->screen_quad_program);
			glDetachShader(_state->screen_quad_program, screenVShader.id());
			glDetachShader(_state->screen_quad_program, screenFShader.id());

            // Set uniforms
			glUseProgram(_state->screen_quad_program);
			glUniform1i(glGetUniformLocation(_state->screen_quad_program, "depth_buffer"), 0);
			glUniform1i(glGetUniformLocation(_state->screen_quad_program, "position_buffer"), 1);
			glUniform1i(glGetUniformLocation(_state->screen_quad_program, "normal_buffer"), 2);
			glUniform1i(glGetUniformLocation(_state->screen_quad_program, "diffuse_buffer"), 3);
			glUniform1i(glGetUniformLocation(_state->screen_quad_program, "specular_buffer"), 4);

            // Create a VAO for debug lines
            glGenVertexArrays(1, &_state->debug_line_vao);
            glBindVertexArray(_state->debug_line_vao);

            // Create a vbo for lines
            glGenBuffers(1, &_state->debug_line_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _state->debug_line_vbo);

            // Line vertex specification
            glEnableVertexAttribArray(DEBUG_LINE_POSITION_ATTRIB_LOCATION);
            glEnableVertexAttribArray(DEBUG_LINE_COLOR_ATTRIB_LOCATION);
            glVertexAttribPointer(
                DEBUG_LINE_POSITION_ATTRIB_LOCATION,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(DebugLineVert),
                nullptr);
            glVertexAttribPointer(
                DEBUG_LINE_COLOR_ATTRIB_LOCATION,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(DebugLineVert),
                (void*)offsetof(DebugLineVert, color_rgb));

            // Create debug line program
            GLShader debug_line_v_shader{ GL_VERTEX_SHADER, config.debug_line_vert_shader };
            GLShader debug_line_f_shader{ GL_FRAGMENT_SHADER, config.debug_line_frag_shader };
            _state->debug_line_program = glCreateProgram();
            glAttachShader(_state->debug_line_program, debug_line_v_shader.id());
            glAttachShader(_state->debug_line_program, debug_line_f_shader.id());

            // Bind vertex attributes
            glBindAttribLocation(_state->debug_line_program, DEBUG_LINE_POSITION_ATTRIB_LOCATION, DEBUG_LINE_POSITION_ATTRIB_NAME);
            glBindAttribLocation(_state->debug_line_program, DEBUG_LINE_COLOR_ATTRIB_LOCATION, DEBUG_LINE_COLOR_ATTRIB_NAME);

            // Link the program and detach shaders
            glLinkProgram(_state->debug_line_program);
            glDetachShader(_state->debug_line_program, debug_line_v_shader.id());
            glDetachShader(_state->debug_line_program, debug_line_f_shader.id());

            // Get uniforms
            _state->debug_line_view_uniform = glGetUniformLocation(_state->debug_line_program, "view");
            _state->debug_line_proj_uniform = glGetUniformLocation(_state->debug_line_program, "projection");

			const auto error = glGetError();
			if (error != GL_NO_ERROR)
			{
				std::cerr << "GLRenderSystem: An error occurred during startup - " << error << std::endl;
			}
		}

		GLRenderSystem::~GLRenderSystem()
		{
			// Todo
		}

		///////////////////
		///   Methods   ///

		void GLRenderSystem::pipeline_register(UpdatePipeline& pipeline)
		{
			pipeline.register_system_fn("gl_render", this, &GLRenderSystem::render_scene);
            pipeline.register_tag_callback(this, &GLRenderSystem::cb_debug_draw);
		}

	    void GLRenderSystem::set_viewport(int width, int height)
	    {
            _state->width = width;
            _state->height = height;

            // Resize depth layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::DEPTH_STENCIL]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_DEPTH_STENCIL_INTERNAL_FORMAT,
                GBUFFER_DEPTH_STENCIL_UPLOAD_FORMAT,
                GBUFFER_DEPTH_STENCIL_UPLOAD_TYPE);

	        // Resize normal layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::NORMAL]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_NORMAL_INTERNAL_FORMAT,
                GBUFFER_NORMAL_UPLOAD_FORMAT,
                GBUFFER_NORMAL_UPLOAD_TYPE);

            // Resize position layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::POSITION]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_POSITION_INTERNAL_FORMAT,
                GBUFFER_POSITION_UPLOAD_FORMAT,
                GBUFFER_POSITION_UPLOAD_TYPE);

            // Resize diffuse layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::DIFFUSE]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_DIFFUSE_INTERNAL_FORMAT,
                GBUFFER_DIFFUSE_UPLOAD_FORMAT,
                GBUFFER_DIFFUSE_UPLOAD_TYPE);

            // Resize specular layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::SPECULAR]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_SPECULAR_INTERNAL_FORMAT,
                GBUFFER_SPECULAR_INTERNAL_FORMAT,
                GBUFFER_SPECULAR_UPLOAD_TYPE);

            // Resize post buffer
            glBindTexture(GL_TEXTURE_2D, _state->post_buffer);
            upload_render_target_data(
                width,
                height,
                POST_BUFFER_INTERNAL_FORMAT,
                POST_BUFFER_UPLOAD_FORMAT,
                POST_BUFFER_UPLOAD_TYPE);
	    }

	    void GLRenderSystem::render_scene(SystemFrame& frame, float current_time, float dt)
		{
			constexpr GLenum DRAW_BUFFERS[] = {
				GBUFFER_POSITION_ATTACHMENT,
				GBUFFER_NORMAL_ATTACHMENT,
				GBUFFER_DIFFUSE_ATTACHMENT,
				GBUFFER_SPECULAR_ATTACHMENT };
			constexpr GLsizei NUM_DRAW_BUFFERS = sizeof(DRAW_BUFFERS) / sizeof(GLenum);

			// Bind the GBuffer and its sub-buffers for drawing
			glBindFramebuffer(GL_FRAMEBUFFER, _state->gbuffer_framebuffer);
			glDrawBuffers(NUM_DRAW_BUFFERS, DRAW_BUFFERS);

			// Clear the GBuffer
            glEnable(GL_STENCIL_TEST);
			glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glDepthFunc(GL_LESS);

			// Create matrices
			bool hasCamera = false;
			Mat4 view;
			Mat4 proj;

			// Get the first camera in the scene
			frame.process_entities([&](
				ProcessingFrame& /*pframe*/,
				EntityId /*entity*/,
				const CTransform3D& transform,
				const CPerspectiveCamera& camera)
			{
				view = transform.get_world_matrix().inverse();
				proj = camera.get_projection_matrix((float)this->_state->width / this->_state->height);
				hasCamera = true;
				return ProcessControl::BREAK;
			});

			// If no camera was found, return
			if (!hasCamera)
			{
				return;
			}

			// DRAW BACKFACE OF RECEIVER
			frame.process_entities([&](
				ProcessingFrame& /*pframe*/,
				EntityId /*entity*/,
				const CTransform3D& transform,
				const CStaticMesh& staticMesh,
                const CLightMaskReceiver& /*recv*/)
			{
				// Get the model matrix
				auto model = transform.get_world_matrix();

				// If the mesh or material is empty, skip it
				if (staticMesh.mesh().empty() || staticMesh.material().empty())
				{
					return;
				}

				// Get the mesh and material
				const auto& mesh = this->_state->find_static_mesh(staticMesh.mesh());
				const auto& material = this->_state->find_material(staticMesh.material());

				// Bind the mesh and material
				mesh.bind();
				GLuint texIndex = GL_TEXTURE0;
				material.bind(texIndex);

				// Upload transformation matrices
				material.set_model_matrix(model);
				material.set_view_matrix(view);
				material.set_projection_matrix(proj);

                // Draw backfaces only
                glCullFace(GL_FRONT);

                // Disable color output
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

                // Set stencil to draw
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				// Draw the mesh
				glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());
			});

            // DRAW BACKFACE OF OBSTRUCTOR
            frame.process_entities([&](
                ProcessingFrame& /*pframe*/,
                EntityId /*entity*/,
                const CTransform3D& transform,
                const CStaticMesh& staticMesh,
                const CLightMaskObstructor& /*obst*/)
            {
                // Get the model matrix
                auto model = transform.get_world_matrix();

                // If the mesh or material is empty, skip it
                if (staticMesh.mesh().empty() || staticMesh.material().empty())
                {
                    return;
                }

                // Get the mesh and material
                const auto& mesh = this->_state->find_static_mesh(staticMesh.mesh());
                const auto& material = this->_state->find_material(staticMesh.material());

                // Bind the mesh and material
                mesh.bind();
                GLuint texIndex = GL_TEXTURE0;
                material.bind(texIndex);

                // Upload transformation matrices
                material.set_model_matrix(model);
                material.set_view_matrix(view);
                material.set_projection_matrix(proj);

                // Enable color output
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

                // Set to replace stencil buffer with '3' wherever drawn (within backface)
                glStencilFunc(GL_EQUAL, 0x03, 0x01);
                glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());
            });

            // OVERWRITE DEPTH WITH FRONT FACE OF RECEIVER
            frame.process_entities([&](
                ProcessingFrame& /*pframe*/,
                EntityId /*entity*/,
                const CTransform3D& transform,
                const CStaticMesh& staticMesh,
                const CLightMaskReceiver& /*recv*/)
            {
                // Get the model matrix
                auto model = transform.get_world_matrix();

                // If the mesh or material is empty, skip it
                if (staticMesh.mesh().empty() || staticMesh.material().empty())
                {
                    return;
                }

                // Get the mesh and material
                const auto& mesh = this->_state->find_static_mesh(staticMesh.mesh());
                const auto& material = this->_state->find_material(staticMesh.material());

                // Bind the mesh and material
                mesh.bind();
                GLuint texIndex = GL_TEXTURE0;
                material.bind(texIndex);

                // Upload transformation matrices
                material.set_model_matrix(model);
                material.set_view_matrix(view);
                material.set_projection_matrix(proj);

                // Draw front faces only
                glCullFace(GL_BACK);

                // Set depth function to normal
                glDepthFunc(GL_LESS);

                // Set stencil to always pass within backface area, allow drawing where depth fails
                glStencilFunc(GL_LEQUAL, 1, 0xFF);
                glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);

                // Disable color output
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());
            });

            // DRAW FRONT-FACE OF OBSTRUCTOR
            frame.process_entities([&](
                ProcessingFrame& /*pframe*/,
                EntityId /*entity*/,
                const CTransform3D& transform,
                const CStaticMesh& staticMesh,
                const CLightMaskObstructor& /*obst*/)
            {
                // Get the model matrix
                auto model = transform.get_world_matrix();

                // If the mesh or material is empty, skip it
                if (staticMesh.mesh().empty() || staticMesh.material().empty())
                {
                    return;
                }

                // Get the mesh and material
                const auto& mesh = this->_state->find_static_mesh(staticMesh.mesh());
                const auto& material = this->_state->find_material(staticMesh.material());

                // Bind the mesh and material
                mesh.bind();
                GLuint texIndex = GL_TEXTURE0;
                material.bind(texIndex);

                // Upload transformation matrices
                material.set_model_matrix(model);
                material.set_view_matrix(view);
                material.set_projection_matrix(proj);

                // Set to allow drawing where depth test fails
                glStencilFunc(GL_LEQUAL, 1, 0xFF);
                glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);

                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());
            });

            // DRAW FRONT FACE OF RECEIVER
            frame.process_entities([&](
                ProcessingFrame& /*pframe*/,
                EntityId /*entity*/,
                const CTransform3D& transform,
                const CStaticMesh& staticMesh,
                const CLightMaskReceiver& /*recv*/)
            {
                // Get the model matrix
                auto model = transform.get_world_matrix();

                // If the mesh or material is empty, skip it
                if (staticMesh.mesh().empty() || staticMesh.material().empty())
                {
                    return;
                }

                // Get the mesh and material
                const auto& mesh = this->_state->find_static_mesh(staticMesh.mesh());
                const auto& material = this->_state->find_material(staticMesh.material());

                // Bind the mesh and material
                mesh.bind();
                GLuint texIndex = GL_TEXTURE0;
                material.bind(texIndex);

                // Upload transformation matrices
                material.set_model_matrix(model);
                material.set_view_matrix(view);
                material.set_projection_matrix(proj);

                // Set depth function to always pass
                glDepthFunc(GL_ALWAYS);

                // Set stencil to always pass within backface area
                glStencilFunc(GL_EQUAL, 1, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

                // Enable color output
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

                // Draw the mesh
                glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());
            });

            // Draw debug lines
            glBindVertexArray(_state->debug_line_vao);
            glUseProgram(_state->debug_line_program);
            glBindBuffer(GL_ARRAY_BUFFER, _state->debug_line_vbo);
            glBufferData(
                GL_ARRAY_BUFFER,
                _state->frame_debug_lines.size() * sizeof(DebugLineVert),
                _state->frame_debug_lines.data(),
                GL_DYNAMIC_DRAW);
		    glUniformMatrix4fv(_state->debug_line_view_uniform, 1, GL_FALSE, view.vec());
		    glUniformMatrix4fv(_state->debug_line_proj_uniform, 1, GL_FALSE, proj.vec());
		    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(_state->frame_debug_lines.size()));
		    _state->frame_debug_lines.clear();

			// Bind the default framebuffer for drawing
			glBindFramebuffer(GL_FRAMEBUFFER, _state->default_framebuffer);

			// Disable depth and culling, and clear the frame
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);
			glClear(GL_COLOR_BUFFER_BIT);

			// Bind the screen quad
			glBindVertexArray(_state->sprite_vao);
			glUseProgram(_state->screen_quad_program);

			// Bind the GBuffer's sub-buffers as textures for reading
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::DEPTH_STENCIL]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::POSITION]);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::NORMAL]);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::DIFFUSE]);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::SPECULAR]);

			// Draw the screen quad
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

	    void GLRenderSystem::cb_debug_draw(
            SystemFrame& /*frame*/,
            const FDebugDraw& tag,
            ComponentId /*component*/)
	    {
            // Reserve space for line verts
            _state->frame_debug_lines.reserve(_state->frame_debug_lines.size() + tag.lines.size() * 2);

            // For each line
            for (auto line : tag.lines)
            {
                // Create start vert
                DebugLineVert start_vert;
                start_vert.world_position = line.world_start;
                start_vert.color_rgb = Vec3{
                    static_cast<Scalar>(line.color.red()) / 255,
                    static_cast<Scalar>(line.color.green()) / 255,
                    static_cast<Scalar>(line.color.blue()) / 255 };

                // Create end vert
                DebugLineVert end_vert;
                end_vert.world_position = line.world_end;
                end_vert.color_rgb = start_vert.color_rgb;

                // Add to the buffer
                _state->frame_debug_lines.push_back(start_vert);
                _state->frame_debug_lines.push_back(end_vert);
            }
	    }
	}
}
