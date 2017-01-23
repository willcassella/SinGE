// GLRenderSystem.h
#pragma once

#include <memory>
#include <Core/Reflection/Reflection.h>
#include <Engine/Scene.h>
#include "build.h"

namespace sge
{
	struct SystemFrame;

	namespace gl_render
	{
		struct Config;

		struct SGE_GLRENDER_API GLRenderSystem
		{
			SGE_REFLECTED_TYPE;
			struct State;

			////////////////////////
			///   Constructors   ///
		public:

			GLRenderSystem(const Config& config);
			~GLRenderSystem();

			///////////////////
			///   Methods   ///
		public:

			void pipeline_register(UpdatePipeline& pipeline);

            void set_viewport(int width, int height);

		private:

			void render_scene(SystemFrame& frame, float current_time, float dt);

			////////////////
			///   Data   ///
		private:

			std::unique_ptr<State> _state;
		};
	}
}
