// GLRenderSystem.cpp

#include <iostream>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Engine/Components/CTransform3D.h>
#include <Engine/Components/Display/CStaticMesh.h>
#include <Engine/Components/Display/CCamera.h>
#include <Engine/Scene.h>
#include <Engine/SystemFrame.h>
#include "../private/GLRenderSystemState.h"

SGE_REFLECT_TYPE(sge::GLRenderSystem);

namespace sge
{
	static void create_gbuffer_layer(
		GLuint layer,
		GLenum attachment,
		GLsizei width,
		GLsizei height,
		GLenum internalFormat,
		GLenum format,
		GLenum type)
	{
		glBindTexture(GL_TEXTURE_2D, layer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, layer, 0);
	}

	static constexpr GLenum GBUFFER_DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT;
	static constexpr GLenum GBUFFER_POSITION_ATTACHMENT = GL_COLOR_ATTACHMENT0;
	static constexpr GLenum GBUFFER_NORMAL_ATTACHMENT = GL_COLOR_ATTACHMENT1;
	static constexpr GLenum GBUFFER_DIFFUSE_ATTACHMENT = GL_COLOR_ATTACHMENT2;
	static constexpr GLenum GBUFFER_SPECULAR_ATTACHMENT = GL_COLOR_ATTACHMENT3;

	////////////////////////
	///   Constructors   ///

	GLRenderSystem::GLRenderSystem(uint32 width, uint32 height)
	{
		// Create the internal state object
		_state = std::make_unique<GLRenderSystem::State>();
		_state->width = static_cast<GLsizei>(width);
		_state->height = static_cast<GLsizei>(height);
		_render_fn_token = Scene::NULL_SYSTEM_TOKEN;

		// Initialize GLEW
		glewExperimental = GL_TRUE;
		glewInit();
		glGetError(); // Sometimes GLEW initialization generates an error, pop it off the stack.

		// Initialize OpenGL
		glClearColor(0, 0, 0, 1);
		glClearDepth(1.f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_STENCIL_TEST);

		// Get the default framebuffer
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_state->default_framebuffer);

		// Create a framebuffer for deferred rendering (GBuffer)
		glGenFramebuffers(1, &_state->gbuffer_framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, _state->gbuffer_framebuffer);
		glGenTextures(GBufferLayer::NUM_LAYERS, _state->gbuffer_layers.data());

		// Create gbuffer depth layer
		create_gbuffer_layer(
			_state->gbuffer_layers[GBufferLayer::DEPTH],
			GBUFFER_DEPTH_ATTACHMENT,
			_state->width,
			_state->height,
			GL_DEPTH_COMPONENT32,
			GL_DEPTH_COMPONENT,
			GL_UNSIGNED_INT);

		// Create gbuffer position layer
		create_gbuffer_layer(
			_state->gbuffer_layers[GBufferLayer::POSITION],
			GBUFFER_POSITION_ATTACHMENT,
			_state->width,
			_state->height,
			GL_RGB32F,
			GL_RGB,
			GL_FLOAT);

		// Create gbuffer normal layer
		create_gbuffer_layer(
			_state->gbuffer_layers[GBufferLayer::NORMAL],
			GBUFFER_NORMAL_ATTACHMENT,
			_state->width,
			_state->height,
			GL_RGB32F,
			GL_RGB,
			GL_FLOAT);

		// Create gbuffer diffuse layer
		create_gbuffer_layer(
			_state->gbuffer_layers[GBufferLayer::DIFFUSE],
			GBUFFER_DIFFUSE_ATTACHMENT,
			_state->width,
			_state->height,
			GL_RGBA8,
			GL_RGBA,
			GL_UNSIGNED_BYTE);

		// Create gbuffer specular layer
		create_gbuffer_layer(
			_state->gbuffer_layers[GBufferLayer::SPECULAR],
			GBUFFER_SPECULAR_ATTACHMENT,
			_state->width,
			_state->height,
			GL_R32F,
			GL_RED,
			GL_FLOAT);

		// Make sure the GBuffer was constructed successfully
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "GLRenderSystem: Error creating GBuffer." << std::endl;
		}

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

		// Vertex specification
		glEnableVertexAttribArray(GLMaterial::POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(GLMaterial::TEXCOORD_ATTRIB_LOCATION);
		glVertexAttribPointer(GLMaterial::POSITION_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
		glVertexAttribPointer(GLMaterial::TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

		// Create and upload a shader program for the screen quad
		GLShader screenVShader{ GL_VERTEX_SHADER, "Content/Shaders/viewport.vert" };
		GLShader screenFShader{ GL_FRAGMENT_SHADER, "Content/Shaders/viewport.frag" };
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

		glUseProgram(_state->screen_quad_program);
		glUniform1i(glGetUniformLocation(_state->screen_quad_program, "depth_buffer"), 0);
		glUniform1i(glGetUniformLocation(_state->screen_quad_program, "position_buffer"), 1);
		glUniform1i(glGetUniformLocation(_state->screen_quad_program, "normal_buffer"), 2);
		glUniform1i(glGetUniformLocation(_state->screen_quad_program, "diffuse_buffer"), 3);
		glUniform1i(glGetUniformLocation(_state->screen_quad_program, "specular_buffer"), 4);

		auto error = glGetError();
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

	void GLRenderSystem::register_with_scene(Scene& scene)
	{
		_render_fn_token = scene.register_system_fn(this, &GLRenderSystem::render_scene);
	}

	void GLRenderSystem::unregister_with_scene(Scene& scene)
	{
		scene.unregister_system_fn(_render_fn_token);
		_render_fn_token = Scene::NULL_SYSTEM_TOKEN;
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
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		// Render each static mesh in the world
		frame.process_entities([&](
			ProcessingFrame& /*pframe*/,
			EntityId /*entity*/,
			const CTransform3D& transform,
			const CStaticMesh& staticMesh)
		{
			// Get the model matrix
			auto model = transform.get_world_matrix();

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

			// Draw the mesh
			glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices());
		});

		// Bind the default framebuffer for drawing
		glBindFramebuffer(GL_FRAMEBUFFER, _state->default_framebuffer);

		// Clear the frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind the screen quad
		glBindVertexArray(_state->sprite_vao);
		glUseProgram(_state->screen_quad_program);

		// Bind the GBuffer's sub-buffers as textures for reading
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::DEPTH]);
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
}
