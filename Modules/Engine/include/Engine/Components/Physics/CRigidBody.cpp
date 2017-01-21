// CRigidBody.cpp

#include <Core/Math/Vec3.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../../include/Engine/Components/Physics/CRigidBody.h"
#include "../../../../include/Engine/Scene.h"
#include "../../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../../include/Engine/ProcessingFrame.h"

SGE_REFLECT_TYPE(sge::CRigidBody)
.property("kinematic", &CRigidBody::kinematic, &CRigidBody::prop_set_kinematic)
.property("mass", &CRigidBody::mass, &CRigidBody::mass)
.property("friction", &CRigidBody::friction, &CRigidBody::friction)
.property("rolling_friction", &CRigidBody::rolling_friction, &CRigidBody::rolling_friction)
.property("linear_motion_factor", &CRigidBody::linear_motion_factor, &CRigidBody::linear_motion_factor)
.property("angular_motion_factor", &CRigidBody::angular_motion_factor, &CRigidBody::angular_motion_factor);

SGE_REFLECT_TYPE(sge::CRigidBody::FKinematicChanged);

namespace sge
{
    struct CRigidBody::Data
    {
        ///////////////////
        ///   Methods   ///
    public:

        void to_archive(ArchiveWriter& writer) const
        {
            writer.object_member("k", kinematic);
            writer.object_member("m", mass);
            writer.object_member("f", friction);
            writer.object_member("rf", rolling_friction);
            writer.object_member("lin", linear_motion_factor);
            writer.object_member("ang", angular_motion_factor);
        }

        void from_archive(ArchiveReader& reader)
        {
            reader.object_member("k", kinematic);
            reader.object_member("m", mass);
            reader.object_member("f", friction);
            reader.object_member("rf", rolling_friction);
            reader.object_member("lin", linear_motion_factor);
            reader.object_member("ang", angular_motion_factor);
        }

        //////////////////
        ///   Fields   ///
    public:

        bool kinematic;

        float mass;

        float friction;

        float rolling_friction;

        Vec3 linear_motion_factor;

        Vec3 angular_motion_factor;
    };

    CRigidBody::CRigidBody(ProcessingFrame& pframe, EntityId entity, Data& data)
        : TComponentInterface<sge::CRigidBody>(pframe, entity),
        _data(&data)
    {
    }

    void CRigidBody::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CRigidBody, Data>>());
    }

    bool CRigidBody::kinematic() const
    {
        return _data->kinematic;
    }

    void CRigidBody::enable_kinematic()
    {
        if (!_data->kinematic)
        {
            _data->kinematic = true;
            processing_frame().create_tag(id(), FKinematicChanged{});
        }
    }

    void CRigidBody::disable_kinematic()
    {
        if (_data->kinematic)
        {
            _data->kinematic = false;
            processing_frame().create_tag(id(), FKinematicChanged{});
        }
    }

    float CRigidBody::mass() const
    {
        return _data->mass;
    }

    void CRigidBody::mass(float value)
    {
        checked_setter(value, _data->mass);
    }

    float CRigidBody::friction() const
    {
        return _data->friction;
    }

    void CRigidBody::friction(float value)
    {
        checked_setter(value, _data->friction);
    }

    float CRigidBody::rolling_friction() const
    {
        return _data->rolling_friction;
    }

    void CRigidBody::rolling_friction(float value)
    {
        checked_setter(value, _data->rolling_friction);
    }

    Vec3 CRigidBody::linear_motion_factor() const
    {
        return _data->linear_motion_factor;
    }

    void CRigidBody::linear_motion_factor(const Vec3& value)
    {
        checked_setter(value, _data->linear_motion_factor);
    }

    Vec3 CRigidBody::angular_motion_factor() const
    {
        return _data->angular_motion_factor;
    }

    void CRigidBody::angular_motion_factor(const Vec3& value)
    {
        checked_setter(value, _data->angular_motion_factor);
    }

    void CRigidBody::prop_set_kinematic(bool value)
    {
        if (value)
        {
            enable_kinematic();
        }
        else
        {
            disable_kinematic();
        }
    }
}
