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

		color::RGBA8 fade_color() const;

		void fade_color(color::RGBA8 value);

		float fade_duration() const;

		void fade_duration(float value);

		const std::string& level_path() const;

		void level_path(std::string value);

		void trigger() const;

		/*--- Fields ---*/
	private:

		color::RGBA8 _fade_color = color::RGBA8::black();
		float _fade_duration = 0.f;
		std::string _level_path;
		NodeId _node_id;
		SharedData* _shared_data;
	};
}
