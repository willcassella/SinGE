// CRigidBody.h
#pragma once

#include "../../Component.h"

namespace sge
{
    class SGE_ENGINE_API CRigidBody final : public TComponentInterface<CRigidBody>
    {
    public:

        struct SGE_ENGINE_API FKinematicChanged
        {
            SGE_REFLECTED_TYPE;
        };

        struct Data;
        SGE_REFLECTED_TYPE;

        ////////////////////////
        ///   Constructors   ///
    public:

        CRigidBody(ProcessingFrame& pframe, EntityId entity, Data& data);

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);

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

        //////////////////
        ///   Fields   ///
    private:

        Data* _data;
    };
}
