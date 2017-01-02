// GLRenderSystemState.cpp

#include <fstream>
#include <iostream>
#include <Resource/Archives/JsonArchive.h>
#include <Core/Util/StringUtils.h>
#include "../private/GLRenderSystemState.h"

namespace sge
{
	const GLShader& GLRenderSystem::State::find_shader(const std::string& path)
	{
		auto iter = shaders.find(path);
		if (iter == shaders.end())
		{
			// Figure out the type of the shader
			GLenum type;
			if (string_ends_with(path, ".vert"))
			{
				type = GL_VERTEX_SHADER;
			}
			else if (string_ends_with(path, ".frag"))
			{
				type = GL_FRAGMENT_SHADER;
			}
			else
			{
				std::cerr << "GLRenderSystem: Shader file extension not recognized.";
				assert(false);
			}

			GLShader shader{ type, path };
			return shaders.insert(std::make_pair(std::move(path), std::move(shader))).first->second;
		}
		else
		{
			return iter->second;
		}
	}

	const GLMaterial& GLRenderSystem::State::find_material(const std::string& path)
	{
		auto iter = materials.find(path);
		if (iter == materials.end())
		{
			// Load the material
			JsonArchive archive;
			archive.from_file(path.c_str());
			Material mat;
			archive.deserialize_root(mat);

			GLMaterial material{ *this, mat };
			return materials.insert(std::make_pair(std::move(path), std::move(material))).first->second;
		}
		else
		{
			return iter->second;
		}
	}

	const GLStaticMesh& GLRenderSystem::State::find_static_mesh(const std::string& path)
	{
		auto iter = static_meshes.find(path);
		if (iter == static_meshes.end())
		{
			// Load the mesh from the file
			StaticMesh staticMesh;
			staticMesh.from_file(path.c_str());

			// Create a GLStaticMesh from that mesh
			GLStaticMesh mesh{ staticMesh };
			return static_meshes.insert(std::make_pair(std::move(path), std::move(mesh))).first->second;
		}
		else
		{
			return iter->second;
		}
	}

	const GLTexture2D& GLRenderSystem::State::find_texture_2d(const std::string& path)
	{
		auto iter = textures.find(path);
		if (iter == textures.end())
		{
			GLTexture2D texture{ Texture{ path } };
			return textures.insert(std::make_pair(std::move(path), std::move(texture))).first->second;
		}
		else
		{
			return iter->second;
		}
	}
}
