// CLightMaskObstructor.h
#pragma once

#include "../../Component.h"

namespace sge
{
    struct SGE_ENGINE_API CLightMaskObstructor
    {
        SGE_REFLECTED_TYPE;
		struct SharedData;

		////////////////////////
		///   Constructors   ///
	public:

		explicit CLightMaskObstructor(NodeId node, SharedData& data);

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);

		void to_archive(ArchiveWriter& writer) const;

		void from_archive(ArchiveReader& reader);

		NodeId node() const;

		//////////////////
		///   Fields   ///
	private:

		NodeId _node;
    };
}
