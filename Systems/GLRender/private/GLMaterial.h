// GLMaterial.h
#pragma once

#include <Core/Math/Mat4.h>
#include <Engine/Resources/Material.h>
#include "../include/GLRender/GLRenderSystem.h"
#include "GLTexture2D.h"

namespace sge
{
	struct GLMaterial
	{
		static constexpr GLint POSITION_ATTRIB_LOCATION = 0;
		static constexpr GLint NORMAL_ATTRIB_LOCATION = 1;
		static constexpr GLint TEXCOORD_ATTRIB_LOCATION = 2;
		static constexpr CString POSITION_ATTRIB_NAME = "v_position";
		static constexpr CString NORMAL_ATTRIB_NAME = "v_normal";
		static constexpr CString TEXCOORD_ATTRIB_NAME = "v_texcoord";
		static constexpr GLint MODEL_UNIFORM_LOCATION = 0;
		static constexpr GLint VIEW_UNIFORM_LOCATION = 1;
		static constexpr GLint PROJ_UNIFORM_LOCATION = 2;
		static constexpr CString MODEL_UNIFORM_NAME = "model";
		static constexpr CString VIEW_UNIFORM_NAME = "view";
		static constexpr CString PROJ_UNIFORM_NAME = "proj";
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

		//////////////////
		///   Fields   ///
	private:

		Id _id;
		GLuint _model;
		GLuint _view;
		GLuint _projection;
		std::unordered_map<GLint, float> _float_params;
		std::unordered_map<GLint, Vec2> _vec2_params;
		std::unordered_map<GLint, Vec3> _vec3_params;
		std::unordered_map<GLint, Vec4> _vec4_params;
		std::unordered_map<GLint, GLTexture2D::Id> _texture_params;
	};
}
