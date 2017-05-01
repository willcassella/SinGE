// CLevelPortal.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Gameplay/CLevelPortal.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"

SGE_REFLECT_TYPE(sge::CLevelPortal)
.implements<IToArchive>()
.implements<IFromArchive>()
.property("node", &CLevelPortal::node, nullptr)
.property("fade_color", &CLevelPortal::fade_color, &CLevelPortal::fade_color)
.property("fade_duration", &CLevelPortal::fade_duration, &CLevelPortal::fade_duration)
.property("level_path", &CLevelPortal::level_path, &CLevelPortal::level_path);

namespace sge
{
	struct CLevelPortal::SharedData : CSharedData<CLevelPortal>
	{
		/*--- Constructors ---*/
	public:

		SharedData()
			: change_level_channel(sizeof(EChangeLevel), 1)
		{
		}

		void reset()
		{
			change_level_channel.clear();
			CSharedData<CLevelPortal>::reset();
		}

		void on_end_update_frame()
		{
			change_level_channel.clear();
			CSharedData<CLevelPortal>::on_end_update_frame();
		}

		EventChannel* get_event_channel(const char* name)
		{
			if (strcmp(name, "change_level") == 0)
			{
				return &change_level_channel;
			}

			return CSharedData<CLevelPortal>::get_event_channel(name);
		}

		/*--- Fields ---*/
	public:

		EventChannel change_level_channel;
	};

	CLevelPortal::CLevelPortal(NodeId node_id, SharedData& shared_data)
		: _node_id(node_id),
		_shared_data(&shared_data)
	{
	}

	void CLevelPortal::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CLevelPortal, SharedData>>());
	}

	void CLevelPortal::to_archive(ArchiveWriter& writer) const
	{
		writer.as_object();
		writer.object_member("fc", _fade_color);
		writer.object_member("fd", _fade_duration);
		writer.object_member("lp", _level_path);
	}

	void CLevelPortal::from_archive(ArchiveReader& reader)
	{
		reader.object_member("fc", _fade_color);
		reader.object_member("fd", _fade_duration);
		reader.object_member("lp", _level_path);
	}

	NodeId CLevelPortal::node() const
	{
		return _node_id;
	}

	color::RGBA8 CLevelPortal::fade_color() const
	{
		return _fade_color;
	}

	void CLevelPortal::fade_color(color::RGBA8 value)
	{
		_fade_color = value;
		_shared_data->set_modified(_node_id, this, "fade_color");
	}

	float CLevelPortal::fade_duration() const
	{
		return _fade_duration;
	}

	void CLevelPortal::fade_duration(float value)
	{
		_fade_duration = value;
		_shared_data->set_modified(_node_id, this, "fade_duration");
	}

	const std::string& CLevelPortal::level_path() const
	{
		return _level_path;
	}

	void CLevelPortal::level_path(std::string value)
	{
		_level_path = std::move(value);
		_shared_data->set_modified(_node_id, this, "level_path");
	}

	void CLevelPortal::trigger() const
	{
		EChangeLevel event;
		event.component = this;
		_shared_data->change_level_channel.append(&event, 1);
	}
}
