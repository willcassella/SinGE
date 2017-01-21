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

        Vec3 linear_motion_factor() const;

        void linear_motion_factor(const Vec3& value);

        Vec3 angular_motion_factor() const;

        void angular_motion_factor(const Vec3& value);

    private:

        void prop_set_kinematic(bool value);

        //////////////////
        ///   Fields   ///
    private:

        Data* _data;
    };
}
