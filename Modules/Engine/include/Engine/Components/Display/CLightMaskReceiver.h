// CLightMaskReceiver.h
#pragma once

#include "../../Component.h"

namespace sge
{
    struct SGE_ENGINE_API CLightMaskReceiver
    {
        SGE_REFLECTED_TYPE;
		struct SharedData;

		////////////////////////
		///   Constructors   ///
	public:

		explicit CLightMaskReceiver(NodeId id, SharedData& data);

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
