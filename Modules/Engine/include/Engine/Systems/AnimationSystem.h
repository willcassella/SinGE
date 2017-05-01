// AnimationSystem.h
#pragma once

#include "../SystemFrame.h"

namespace sge
{
	struct SGE_ENGINE_API AnimationSystem
	{
		/*--- Methods ---*/
	public:

		void register_pipeline(UpdatePipeline& pipeline);

	private:

		void animation_update(Scene& scene, SystemFrame& frame);

		void animation_apply(Scene& scene, SystemFrame& frame);
	};
}
