// GLMaterial.h - Copyright 2013-2016 Will Cassella, All Rights Reserved
#pragma once

#include <Engine/Resources/Material.h>
#include "config.h"

namespace sge
{
	struct SGE_GLRENDER_API GLMaterial final
	{
		////////////////////////
		///   Constructors   ///
	public:

		/** Constructs an OpenGL material from the given Material asset. */
		GLMaterial(GLRenderSystem& renderer, const Material& mat);

		GLMaterial(GLMaterial&& move);
		~GLMaterial();

		///////////////////
		///   Methods   ///
	public:

		/** Returns the ID of this Material. */
		FORCEINLINE BufferID get_id() const
		{
			return this->_id;
		}

		/** Sets this material as the current active material, with the given instance parameters. */
		void bind(GLRenderSystem& renderer, const Table<String, Material::Param>& instanceParams);

	private:

		/** Uploads the given parameters to this material. */
		void upload_params(GLRenderSystem& renderer, const Table<String, Material::Param>& params, uint32& texIndex);

		/** Uploads a scalar parameter to the given location. */
		void upload_param(int32 location, float value) const;

		/** Uploads a Vec2 parameter to the given location. */
		void upload_param(int32 location, Vec2 value) const;

		/** Uploads a Vec3 parameter to the given location. */
		void upload_param(int32 location, Vec3 value) const;

		/** Uploads a Vec4 location to the given location. */
		void upload_param(int32 location, Vec4 value) const;

		/** Uploads a texture parameter to the given location. */
		void upload_param(int32 location, ResourceHandle<Texture> value);

		////////////////
		///   Data   ///
	private:

		Table<String, Material::Param> _params;
		BufferID _id;
		BufferID _model;
		BufferID _view;
		BufferID _projection;
	};
}
