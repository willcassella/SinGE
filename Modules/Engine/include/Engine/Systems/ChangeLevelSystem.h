// ChangeLevelSystem.h
#pragma once

#include "../Component.h"

namespace sge
{
	struct UpdatePipeline;

	struct SGE_ENGINE_API ChangeLevelSystem
	{
		/*--- Methods ---*/
	public:

		void pipeline_register(UpdatePipeline& pipeline);

		void initialize_subscriptions(Scene& scene);

		bool requires_change_level() const;

		const std::string& change_level_target() const;

		void reset();

	private:

		void check_change_level(Scene& scene, SystemFrame& frame);

		/*--- Fields ---*/
	private:

		std::string _change_level_target;
		EventChannel* _change_level_channel = nullptr;
		EventChannel::SubscriberId _change_level_sid = EventChannel::INVALID_SID;
	};
}
