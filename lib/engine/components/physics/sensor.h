#pragma once

#include "lib/engine/component.h"

namespace sge
{
    struct SGE_ENGINE_API CSensor
    {
        SGE_REFLECTED_TYPE;
        struct SharedData;

        explicit CSensor(NodeId node, SharedData& shared_data);

        static void register_type(Scene& scene);

        void to_archive(ArchiveWriter& writer) const;

        void from_archive(ArchiveReader& reader);

        NodeId node() const;

    private:
        NodeId _node;
    };
}
