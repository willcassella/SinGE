// CAnimation.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Gameplay/CAnimation.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CAnimation)
.implements<IToArchive>()
.implements<IFromArchive>()
.property("node", &CAnimation::node, nullptr)
.property("index", &CAnimation::index, &CAnimation::index)
.property("duration", &CAnimation::duration, &CAnimation::duration)
.property("animate_position", &CAnimation::animate_position, &CAnimation::animate_position)
.property("init_position", &CAnimation::init_position, &CAnimation::init_position)
.property("target_position", &CAnimation::target_position, &CAnimation::target_position)
.property("animate_rotation", &CAnimation::animate_rotation, &CAnimation::animate_rotation)
.property("init_rotation", &CAnimation::init_rotation, &CAnimation::init_rotation)
.property("target_rotation", &CAnimation::target_rotation, &CAnimation::target_rotation);

namespace sge
{
	struct CAnimation::SharedData : CSharedData<CAnimation>
	{
	};

	CAnimation::CAnimation(NodeId node_id, SharedData& shared_data)
		: _node(node_id),
		_shared_data(&shared_data)
	{
	}

	void CAnimation::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CAnimation, SharedData>>());
	}

	void CAnimation::to_archive(ArchiveWriter& writer) const
	{
		writer.as_object();
		writer.object_member("index", _index);
		writer.object_member("duration", _duration);
		writer.object_member("anim_pos", _animate_position);
		writer.object_member("anim_rot", _animate_rotation);
		writer.object_member("init_pos", _init_position);
		writer.object_member("targ_pos", _target_position);
		writer.object_member("init_rot", _init_rotation);
		writer.object_member("targ_rot", _target_rotation);
	}

	void CAnimation::from_archive(ArchiveReader& reader)
	{
		reader.object_member("index", _index);
		reader.object_member("duration", _duration);
		reader.object_member("anim_pos", _animate_position);
		reader.object_member("anim_rot", _animate_rotation);
		reader.object_member("init_pos", _init_position);
		reader.object_member("targ_pos", _target_position);
		reader.object_member("init_rot", _init_rotation);
		reader.object_member("targ_rot", _target_rotation);
	}

	NodeId CAnimation::node() const
	{
		return _node;
	}

	float CAnimation::index() const
	{
		return _index;
	}

	void CAnimation::index(float value)
	{
		_index = value;
		set_modified("index");
	}

	float CAnimation::duration() const
	{
		return _duration;
	}

	void CAnimation::duration(float value)
	{
		_duration = value;
		set_modified("duration");
	}

	bool CAnimation::animate_position() const
	{
		return _animate_position;
	}

	void CAnimation::animate_position(bool value)
	{
		_animate_position = value;
		set_modified("animate_position");
	}

	Vec3 CAnimation::init_position() const
	{
		return _init_position;
	}

	void CAnimation::init_position(Vec3 value)
	{
		_init_position = value;
		set_modified("init_position");
	}

	Vec3 CAnimation::target_position() const
	{
		return _target_position;
	}

	void CAnimation::target_position(Vec3 value)
	{
		_target_position = value;
		set_modified("target_position");
	}

	bool CAnimation::animate_rotation() const
	{
		return _animate_rotation;
	}

	void CAnimation::animate_rotation(bool value)
	{
		_animate_rotation = value;
		set_modified("animate_rotation");
	}

	Quat CAnimation::init_rotation() const
	{
		return _init_rotation;
	}

	void CAnimation::init_rotation(Quat value)
	{
		_init_rotation = value;
		set_modified("init_rotation");
	}

	Quat CAnimation::target_rotation() const
	{
		return _target_rotation;
	}

	void CAnimation::target_rotation(Quat value)
	{
		_target_rotation = value;
		set_modified("target_rotation");
	}

	void CAnimation::set_modified(const char* prop_name)
	{
		_shared_data->set_modified(_node, this, prop_name);
	}
}
