// GLRender.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "config.h"

namespace sge
{
	namespace gl
	{
		using BufferID = uint32;
	}

	struct SGE_GLRENDER_API GLRenderSystem
	{
		SGE_REFLECTED_TYPE;

		///////////////////
		///   Methods   ///
	public:

		void init_selectors(const Scene& /*scene*/) const
		{
		}

		////////////////
		///   Data   ///
	private:

		gl::BufferID _gbuffer;
	};
}
