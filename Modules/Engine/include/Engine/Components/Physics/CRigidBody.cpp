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
.property("spinning_friction", &CRigidBody::spinning_friction, &CRigidBody::spinning_friction)
.property("linear_damping", &CRigidBody::linear_damping, &CRigidBody::linear_damping)
.property("angular_damping", &CRigidBody::angular_damping, &CRigidBody::angular_damping);

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
            writer.object_member("sf", spinning_friction);
            writer.object_member("lin", linear_damping);
            writer.object_member("ang", angular_damping);
        }

        void from_archive(ArchiveReader& reader)
        {
            reader.object_member("k", kinematic);
            reader.object_member("m", mass);
            reader.object_member("f", friction);
            reader.object_member("rf", rolling_friction);
            reader.object_member("sf", spinning_friction);
            reader.object_member("lin", linear_damping);
            reader.object_member("ang", angular_damping);
        }

        //////////////////
        ///   Fields   ///
    public:

        bool kinematic = false;

        float mass = 1.f;

        float friction = 0.5f;

        float rolling_friction = 0.f;

        float spinning_friction = 0.f;

        float linear_damping = 0.f;

        float angular_damping = 0.f;
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

    float CRigidBody::spinning_friction() const
    {
        return _data->spinning_friction;
    }

    void CRigidBody::spinning_friction(float value)
    {
        checked_setter(value, _data->spinning_friction);
    }

    float CRigidBody::linear_damping() const
    {
        return _data->linear_damping;
    }

    void CRigidBody::linear_damping(float value)
    {
        checked_setter(value, _data->linear_damping);
    }

    float CRigidBody::angular_damping() const
    {
        return _data->angular_damping;
    }

    void CRigidBody::angular_damping(float value)
    {
        checked_setter(value, _data->angular_damping);
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
