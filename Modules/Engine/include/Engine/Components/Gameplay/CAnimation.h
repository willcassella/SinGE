// CAnimation.h
#pragma once

#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CAnimation
	{
		SGE_REFLECTED_TYPE;
		struct SharedData;

		/*--- Constructors ---*/
	public:

		CAnimation(NodeId node_id, SharedData& shared_data);

		/*--- Fields ---*/
	public:

		static void register_type(Scene& scene);

		void to_archive(ArchiveWriter& writer) const;

		void from_archive(ArchiveReader& reader);

		NodeId node() const;

		float index() const;

		void index(float value);

		float duration() const;

		void duration(float value);

		bool animate_position() const;

		void animate_position(bool value);

		Vec3 init_position() const;

		void init_position(Vec3 value);

		Vec3 target_position() const;

		void target_position(Vec3 value);

		bool animate_rotation() const;

		void animate_rotation(bool value);

		Quat init_rotation() const;

		void init_rotation(Quat value);

		Quat target_rotation() const;

		void target_rotation(Quat value);

	private:

		void set_modified(const char* prop_name);

		/*--- Fields ---*/
	private:

		float _index = 0.f;
		float _duration = 0.f;
		bool _animate_position = true;
		bool _animate_rotation = true;
		Vec3 _init_position;
		Vec3 _target_position;
		Quat _init_rotation;
		Quat _target_rotation;
		NodeId _node;
		SharedData* _shared_data;
	};
}
