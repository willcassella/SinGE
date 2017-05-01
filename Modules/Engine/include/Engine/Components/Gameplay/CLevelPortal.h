// CLevelPortal.h
#pragma once

#include <Resource/Misc/Color.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CLevelPortal
	{
		SGE_REFLECTED_TYPE;
		struct SharedData;

		struct EChangeLevel
		{
			const CLevelPortal* component;
		};

		/*--- Constructors ---*/
	public:

		CLevelPortal(NodeId node_id, SharedData& shared_data);

		/*--- Methods ---*/
	public:

		static void register_type(Scene& scene);

		void to_archive(ArchiveWriter& writer) const;

		void from_archive(ArchiveReader& reader);

		NodeId node() const;

		bool gamma_fade() const;

		void gamma_fade(bool value);

		bool brightness_fade() const;

		void brightness_fade(bool value);

		float fade_duration() const;

		void fade_duration(float value);

		const std::string& level_path() const;

		void level_path(std::string value);

		void trigger() const;

		/*--- Fields ---*/
	private:

		bool _gamma_fade = true;
		bool _brightness_fade = false;
		float _fade_duration = 0.f;
		std::string _level_path;
		NodeId _node_id;
		SharedData* _shared_data;
	};
}
