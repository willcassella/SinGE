// SystemFrame.h
#pragma once

#include <stack>
#include <Core/Functional/UFunction.h>
#include "UpdatePipeline.h"

namespace sge
{
	struct ProcessingFrame;
	struct SceneData;
	struct Scene;

	struct SGE_ENGINE_API SystemFrame
	{
	public:

		SGE_REFLECTED_TYPE;

		/* Only 'Scene' objects may construct SystemFrames. */
		friend Scene;

		////////////////////////
		///   Constructors   ///
	private:

		SystemFrame() = default;
		SystemFrame(SystemFrame&& move) = delete;
		SystemFrame(const SystemFrame& copy) = delete;
		SystemFrame& operator=(const SystemFrame& copy) = delete;
		SystemFrame& operator=(SystemFrame&& move) = delete;

		///////////////////
		///   Methods   ///
	public:

		void yield();

		uint64 frame_id() const;

		float current_time() const;

		float time_delta() const;

		void push(const char* system_name);

		//////////////////
		///   Fields   ///
	private:

		float _current_time = 0.f;
		float _time_delta = 0.f;
		Scene* _scene = nullptr;
		UpdatePipeline* _update_pipeline = nullptr;
		std::vector<SystemInfo*> _job_queue;
	};
}
