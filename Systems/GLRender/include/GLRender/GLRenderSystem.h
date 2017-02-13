// GLRenderSystem.h
#pragma once

#include <memory>
#include <Core/Reflection/Reflection.h>
#include <Engine/Scene.h>
#include <Engine/TagBuffer.h>
#include "build.h"

namespace sge
{
	struct SystemFrame;
    struct FDebugLine;

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

            void cb_debug_draw_line(
                SystemFrame& frame,
                const EntityId* ent_range,
                const TagIndex_t* indices,
                const TagCount_t* counts,
                std::size_t ent_range_len,
                const FDebugLine* lines);

            void cb_new_transform(
                SystemFrame& frame,
                const EntityId* ent_range,
                std::size_t range_len);

            void cb_new_static_mesh(
                SystemFrame& frame,
                const EntityId* ent_range,
                std::size_t range_len);

            void cb_modified_static_mesh(
                SystemFrame& frame,
                const EntityId* ent_range,
                std::size_t range_len);

            void cb_new_lightmask_obstructor(
                SystemFrame& frame,
                const EntityId* ent_range,
                std::size_t range_len);

			////////////////
			///   Data   ///
		private:

			std::unique_ptr<State> _state;
		};
	}
}
