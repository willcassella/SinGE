// CBoxCollider.h
#pragma once

#include <Core/Math/Vec3.h>
#include "../../Component.h"

namespace sge
{
    struct SGE_ENGINE_API CBoxCollider
    {
        SGE_REFLECTED_TYPE;
        struct SharedData;

        /////////////////////////
        ///   Constructors    ///
    public:

        explicit CBoxCollider(NodeId node, SharedData& shared_data);

        ////////////////////
        ////   Methods   ///
    public:

        static void register_type(Scene& scene);

        void to_archive(ArchiveWriter& writer) const;

        void from_archive(ArchiveReader& reader);

        NodeId node() const;

        float width() const;

        void width(float value);

        float height() const;

        void height(float value);

        float depth() const;

        void depth(float value);

        Vec3 shape() const;

        void shape(const Vec3& shape);

    private:

        void set_modified();

        //////////////////
        ///   Fields   ///
    private:

        Vec3 _shape = { 1.f, 1.f, 1.f };
        NodeId _node;
        SharedData* _shared_data = nullptr;
    };
}
