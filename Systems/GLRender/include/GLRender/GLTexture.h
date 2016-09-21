// GLTexture.h - Copyright 2013-2016 Will Cassella, All Rights Reserved
#pragma once

#include <Engine/Resources/Texture.h>
#include "Forwards.h"
#include "config.h"

namespace willow
{
	struct GLRENDER_API GLTexture final
	{
		////////////////////////
		///   Constructors   ///
	public:

		/** Creates an OpenGL texture from the corresponding Texture asset. */
		GLTexture(const Texture& texture);

		GLTexture(GLTexture&& move);
		~GLTexture();

		///////////////////
		///   Methods   ///
	public:

		FORCEINLINE BufferID get_id() const
		{
			return this->_id;
		}

		////////////////
		///   Data   ///
	private:

		BufferID _id;
	};
}
