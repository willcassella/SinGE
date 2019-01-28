// CSensor.h
#pragma once

#include "../../Component.h"

namespace sge
{
    struct SGE_ENGINE_API CSensor
    {
        SGE_REFLECTED_TYPE;
        struct SharedData;

        ////////////////////////
        ///   Constructors   ///
    public:

        explicit CSensor(NodeId node, SharedData& shared_data);

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
