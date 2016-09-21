// GLRenderSystem.h - Copyright 2013-2016 Will Cassella, All Rights Reserved
#pragma once

#include <Engine/Scene.h>
#include <Engine/Resources/Shader.h>
#include <Engine/Resources/Material.h>
#include <Engine/Resources/StaticMesh.h>
#include "GLShader.h"
#include "GLTexture.h"
#include "GLStaticMesh.h"
#include "GLMaterial.h"

namespace sge
{
	class SGE_GLRENDER_API GLRenderSystem
	{
		////////////////////////
		///   Constructors   ///
	public:

		GLRenderSystem(uint32 width, uint32 height);
		~GLRenderSystem();

		///////////////////
		///   Methods   ///
	public:

		void render_scene(const Scene& scene, uint32 views);

		/** Finds or loads a GLShader from the given Shader handle. */
		GLShader& find_shader(std::string path);

		/** Finds or loads a GLMaterial based on the given Texture asset. */
		GLMaterial& find_material(std::string path);

		/** Finds or loads a GLMaterial based on the given Static Mesh asset. */
		GLStaticMesh& find_static_mesh(std::string path);

		////////////////
		///   Data   ///
	private:

		std::unordered_map<std::string, GLShader> _shaders;
		std::unordered_map<std::string, GLMaterial> _materials;
		std::unordered_map<std::string, GLStaticMesh> _staticMeshes;

		// The default framebuffer
		GLInteger _defaultFrameBuffer;

		// GBuffer layers
		GLBufferID _gBuffer;
		GLBufferID _depthBuffer;
		GLBufferID _positionBuffer;
		GLBufferID _diffuseBuffer;
		GLBufferID _normalBuffer;
		GLBufferID _specularBuffer;

		// Screen data
		uint32 _width;
		uint32 _height;

		// Screen quad buffers
		GLBufferID _screenQuadVAO;
		GLBufferID _screenQuadVBO;
		GLBufferID _screenQuadProgram;
	};
}
