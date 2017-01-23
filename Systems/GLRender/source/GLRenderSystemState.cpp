// GLRenderSystemState.cpp

#include <fstream>
#include <iostream>
#include <Resource/Archives/JsonArchive.h>
#include <Core/Util/StringUtils.h>
#include "../private/GLRenderSystemState.h"

namespace sge
{
	namespace gl_render
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
				auto result = archive.from_file(path.c_str());

				// If the material could not be loaded, create a material from the missing material object
				if (!result)
				{
					GLMaterial material{ *this, missing_material };
					return materials.insert(std::make_pair(std::move(path), std::move(material))).first->second;
				}

				// Create the material from the archive
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
				auto result = staticMesh.from_file(path.c_str());

				// If the mesh could not be loaded
				if (!result)
				{
					// Create a GLStaticMesh from the missing mesh object
					GLStaticMesh mesh{ missing_mesh };
					return static_meshes.insert(std::make_pair(std::move(path), std::move(mesh))).first->second;
				}

				// Create a GLStaticMesh from the loaded mesh object
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
}
