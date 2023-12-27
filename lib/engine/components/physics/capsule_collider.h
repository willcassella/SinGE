#pragma once

#include "lib/engine/component.h"

namespace sge
{
    struct SGE_ENGINE_API CCapsuleCollider
    {
        SGE_REFLECTED_TYPE;
        struct SharedData;

        explicit CCapsuleCollider(NodeId node, SharedData& shared_data);

        static void register_type(Scene& scene);

        void to_archive(ArchiveWriter& writer) const;

        void from_archive(ArchiveReader& reader);

        NodeId node() const;

        float radius() const;

        void radius(float value);

        float height() const;

        void height(float value);

    private:
        void set_modified(const char* property_name);

        Vec2 _shape = { 1.f, 1.f };
        NodeId _node;
        SharedData* _shared_data = nullptr;
    };
}
