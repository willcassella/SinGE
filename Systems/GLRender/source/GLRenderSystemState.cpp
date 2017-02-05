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
		GLShader::Id GLRenderSystem::State::get_shader_resource(const std::string& path)
		{
			auto iter = shader_resources.find(path);
			if (iter == shader_resources.end())
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

                // Create the shader
				GLShader shader{ type, path };
                const auto id = shader.id();

                // Put it into the resource table
                shader_resources[path] = id;
                shaders.insert(std::make_pair(id, std::move(shader)));
                return id;
			}
			else
			{
				return iter->second;
			}
		}

		GLMaterial::Id GLRenderSystem::State::get_material_resource(const std::string& path)
		{
			auto iter = material_resources.find(path);
			if (iter == material_resources.end())
			{
				// Load the material
				JsonArchive archive;
				const bool loaded = archive.from_file(path.c_str());

				// If the material could not be loaded, return missing material
				if (!loaded)
				{
                    return missing_material;
				}

				// Create the material from the archive
				Material material;
				archive.deserialize_root(material);
				GLMaterial gl_material{ *this, material };

                // Put it into the resource table
                const auto id = gl_material.id();
                material_resources[path] = id;
                materials.insert(std::make_pair(id, std::move(gl_material)));
                return id;
			}
			else
			{
				return iter->second;
			}
		}

		GLStaticMesh::VAO GLRenderSystem::State::get_static_mesh_resource(const std::string& path)
		{
			auto iter = static_mesh_resources.find(path);
			if (iter == static_mesh_resources.end())
			{
				// Load the mesh from the file
				StaticMesh static_mesh;
				const bool loaded = static_mesh.from_file(path.c_str());

				// If the mesh could not be loaded, return the missing mesh object
				if (!loaded)
				{
                    return missing_mesh;
				}

				// Create a GLStaticMesh from the loaded mesh object
				GLStaticMesh gl_mesh{ static_mesh };

                // Insert it into the resource table
                const auto vao = gl_mesh.vao();
                static_mesh_resources[path] = vao;
                static_meshes.insert(std::make_pair(vao, std::move(gl_mesh)));
                return vao;
			}
			else
			{
				return iter->second;
			}
		}

		GLTexture2D::Id GLRenderSystem::State::get_texture_2d_resource(const std::string& path)
		{
			auto iter = texture_2d_resources.find(path);
			if (iter == texture_2d_resources.end())
			{
                // Load the texture from the file
                Texture texture;
                const auto loaded = texture.from_file(path.c_str());

                // TODO: Have default texture in case texture could not be loaded

                // Create an opengl texture from the texture object
                GLTexture2D gl_texture{ texture };

                // Add it to the resource table
                const auto id = gl_texture.id();
                texture_2d_resources[path] = id;
                texture_2ds.insert(std::make_pair(id, std::move(gl_texture)));
                return id;
			}
			else
			{
				return iter->second;
			}
		}
	}
}
