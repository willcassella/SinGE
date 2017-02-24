// GLMaterial.h
#pragma once

#include <Core/Math/Mat4.h>
#include <Resource/Resources/Material.h>
#include "../include/GLRender/GLRenderSystem.h"
#include "GLTexture2D.h"
#include "Util.h"

namespace sge
{
	namespace gl_render
	{
		struct GLMaterial
		{
			static constexpr GLint POSITION_ATTRIB_LOCATION = 0;
			static constexpr GLint NORMAL_ATTRIB_LOCATION = 1;
            static constexpr GLint TANGENT_ATTRIB_LOCATION = 2;
            static constexpr GLint BITANGENT_SIGN_ATTRIB_LOCATION = 3;
			static constexpr GLint TEXCOORD_ATTRIB_LOCATION = 4;
			static constexpr const char* POSITION_ATTRIB_NAME = "v_position";
			static constexpr const char* NORMAL_ATTRIB_NAME = "v_normal";
            static constexpr const char* TANGENT_ATTRIB_NAME = "v_tangent";
            static constexpr const char* BITANGENT_SIGN_ATTRIB_NAME = "v_bitangent_sign";
			static constexpr const char* TEXCOORD_ATTRIB_NAME = "v_texcoord";
			static constexpr GLint MODEL_UNIFORM_LOCATION = 0;
			static constexpr GLint VIEW_UNIFORM_LOCATION = 1;
			static constexpr GLint PROJ_UNIFORM_LOCATION = 2;
			static constexpr const char* MODEL_UNIFORM_NAME = "model";
			static constexpr const char* VIEW_UNIFORM_NAME = "view";
			static constexpr const char* PROJ_UNIFORM_NAME = "proj";
			using Id = GLuint;

			////////////////////////
			///   Constructors   ///
		public:

			/** Constructs an OpenGL material from the given Material asset. */
			GLMaterial(GLRenderSystem::State& renderState, const Material& mat);

			GLMaterial(GLMaterial&& move);
			~GLMaterial();

			///////////////////
			///   Methods   ///
		public:

			Id id() const
			{
				return _id;
			}

			void bind(GLuint& texIndex) const;

			void set_model_matrix(const Mat4& model) const;

			void set_view_matrix(const Mat4& view) const;

			void set_projection_matrix(const Mat4& projection) const;

			void override_params(GLRenderSystem::State& renderState, const Material::ParamTable& params, GLuint& texIndex) const;

		private:

            GLint get_uniform_location(const char* name, GLDebugOutputMode out_mode = GLDebugOutputMode::ONLY_ERROR) const;

			//////////////////
			///   Fields   ///
		private:

			Id _id;
			GLuint _model;
			GLuint _view;
			GLuint _projection;
            std::unordered_map<GLint, bool> _bool_params;
			std::unordered_map<GLint, float> _float_params;
			std::unordered_map<GLint, Vec2> _vec2_params;
			std::unordered_map<GLint, Vec3> _vec3_params;
			std::unordered_map<GLint, Vec4> _vec4_params;
			std::unordered_map<GLint, GLTexture2D::Id> _texture_params;
		};
	}
}
