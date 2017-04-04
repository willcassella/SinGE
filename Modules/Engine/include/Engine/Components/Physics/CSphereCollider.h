// CSphereCollider.h
#pragma once

#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CSphereCollider
	{
		struct SharedData;
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		CSphereCollider(NodeId node, SharedData& shared_data);

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

		void to_archive(ArchiveWriter& writer) const;

		void from_archive(ArchiveReader& reader);

		NodeId node() const;

		float radius() const;

		void radius(float value);

		//////////////////
		///   Fields   ///
	private:

		float _radius = 1.f;
		NodeId _node;
		SharedData* _shared_data;
	};
}
