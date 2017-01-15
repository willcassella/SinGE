// GLRenderSystem.h
#pragma once

#include <memory>
#include <Core/Reflection/Reflection.h>
#include <Engine/Scene.h>
#include "build.h"

namespace sge
{
	class SystemFrame;

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

			void register_with_scene(Scene& scene);

			void unregister_with_scene(Scene& scene);

		private:

			void render_scene(SystemFrame& frame, float current_time, float dt);

			////////////////
			///   Data   ///
		private:

			std::unique_ptr<State> _state;
			Scene::SystemFnToken _render_fn_token;
		};
	}
}
