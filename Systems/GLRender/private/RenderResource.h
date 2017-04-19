// RenderResource.h
#pragma once

#include <unordered_map>
#include "GLMaterial.h"
#include "GLShader.h"
#include "GLStaticMesh.h"

namespace sge
{
	namespace gl_render
	{
		struct RenderResource
		{
			/* Dynamically loaded resources. */
			std::unordered_map<std::string, gl_material::Material> material_resources;
			std::unordered_map<std::string, gl_static_mesh::StaticMesh> static_mesh_resources;
			std::unordered_map<std::string, GLuint> shader_resources;
			std::unordered_map<std::string, GLuint> texture_2d_resources;

			/* Default resources. */
			gl_material::Material missing_material;
			gl_static_mesh::StaticMesh missing_mesh;
		};

		const gl_material::Material& RenderResource_get_material_resource(
			RenderResource& resources,
			const char* path);

		const gl_static_mesh::StaticMesh& RenderResource_get_static_mesh_resource(
			RenderResource& resources,
			const char* path);

		GLuint RenderResource_get_shader_resource(
			RenderResource& resources,
			const char* path);

		GLuint RenderResource_get_texture_2d_resource(
			RenderResource& resources,
			const char* path,
			bool hdr);
	}
}
