// CRigidBody.h
#pragma once

#include "../../Component.h"

namespace sge
{
    struct SGE_ENGINE_API CRigidBody
    {
        SGE_REFLECTED_TYPE;
        struct SharedData;

        ////////////////////////
        ///   Constructors   ///
    public:

        explicit CRigidBody(NodeId node, SharedData& shared_data);

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);

        void to_archive(ArchiveWriter& writer) const;

        void from_archive(ArchiveReader& reader);

        NodeId node() const;

        bool kinematic() const;

        void enable_kinematic();

        void disable_kinematic();

        float mass() const;

        void mass(float value);

        float friction() const;

        void friction(float value);

        float rolling_friction() const;

        void rolling_friction(float value);

        float spinning_friction() const;

        void spinning_friction(float value);

        float linear_damping() const;

        void linear_damping(float value);

        float angular_damping() const;

        void angular_damping(float value);

    private:

        void prop_set_kinematic(bool value);

        void set_modified(const char* property_name);

        //////////////////
        ///   Fields   ///
    private:

        bool _kinematic = false;
        float _mass = 1.f;
        float _friction = 0.5f;
        float _rolling_friction = 0.f;
        float _spinning_friction = 0.f;
        float _linear_damping = 0.f;
        float _angular_damping = 0.f;
        NodeId _node;
        SharedData* _shared_data = nullptr;
    };
}
