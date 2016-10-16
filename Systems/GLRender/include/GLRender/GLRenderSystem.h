// GLRenderSystem.h
#pragma once

#include <memory>
#include <Core/Reflection/Reflection.h>
#include "config.h"

namespace sge
{
	struct Frame;

	struct SGE_GLRENDER_API GLRenderSystem
	{
		SGE_REFLECTED_TYPE;
		struct State;

		////////////////////////
		///   Constructors   ///
	public:

		GLRenderSystem(uint32 width, uint32 height);
		~GLRenderSystem();

		///////////////////
		///   Methods   ///
	public:

		void render_frame(const Frame& frame);

		////////////////
		///   Data   ///
	private:

		std::unique_ptr<State> _state;
	};
}
