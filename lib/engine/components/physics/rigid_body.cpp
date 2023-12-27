#include "lib/base/reflection/reflection_builder.h"
#include "lib/engine/components/physics/rigid_body.h"
#include "lib/engine/scene.h"
#include "lib/engine/util/basic_component_container.h"
#include "lib/engine/util/shared_data.h"

SGE_REFLECT_TYPE(sge::CRigidBody)
.property("kinematic", &CRigidBody::kinematic, &CRigidBody::prop_set_kinematic)
.property("mass", &CRigidBody::mass, &CRigidBody::mass)
.property("friction", &CRigidBody::friction, &CRigidBody::friction)
.property("rolling_friction", &CRigidBody::rolling_friction, &CRigidBody::rolling_friction)
.property("spinning_friction", &CRigidBody::spinning_friction, &CRigidBody::spinning_friction)
.property("linear_damping", &CRigidBody::linear_damping, &CRigidBody::linear_damping)
.property("angular_damping", &CRigidBody::angular_damping, &CRigidBody::angular_damping);

namespace sge
{
    struct CRigidBody::SharedData : CSharedData<CRigidBody>
    {
    };

    CRigidBody::CRigidBody(NodeId node, SharedData& shared_data)
        : _node(node),
        _shared_data(&shared_data)
    {
    }

    void CRigidBody::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CRigidBody, SharedData>>());
    }

    void CRigidBody::to_archive(ArchiveWriter& writer) const
    {
        writer.object_member("k", _kinematic);
        writer.object_member("m", _mass);
        writer.object_member("f", _friction);
        writer.object_member("rf", _rolling_friction);
        writer.object_member("sf", _spinning_friction);
        writer.object_member("lin", _linear_damping);
        writer.object_member("ang", _angular_damping);
    }

    void CRigidBody::from_archive(ArchiveReader& reader)
    {
        reader.object_member("k", _kinematic);
        reader.object_member("m", _mass);
        reader.object_member("f", _friction);
        reader.object_member("rf", _rolling_friction);
        reader.object_member("sf", _spinning_friction);
        reader.object_member("lin", _linear_damping);
        reader.object_member("ang", _angular_damping);
    }

    NodeId CRigidBody::node() const
    {
        return _node;
    }

    bool CRigidBody::kinematic() const
    {
        return _kinematic;
    }

    void CRigidBody::enable_kinematic()
    {
        if (!_kinematic)
        {
            _kinematic = true;
            set_modified("kinematic");
        }
    }

    void CRigidBody::disable_kinematic()
    {
        if (_kinematic)
        {
            _kinematic = false;
            set_modified("kinematic");
        }
    }

    float CRigidBody::mass() const
    {
        return _mass;
    }

    void CRigidBody::mass(float value)
    {
        if (_mass != value)
        {
            _mass = value;
            set_modified("mass");
        }
    }

    float CRigidBody::friction() const
    {
        return _friction;
    }

    void CRigidBody::friction(float value)
    {
        if (_friction != value)
        {
            _friction = value;
            set_modified("friction");
        }
    }

    float CRigidBody::rolling_friction() const
    {
        return _rolling_friction;
    }

    void CRigidBody::rolling_friction(float value)
    {
        if (_rolling_friction != value)
        {
            _rolling_friction = value;
            set_modified("rolling_friction");
        }
    }

    float CRigidBody::spinning_friction() const
    {
        return _spinning_friction;
    }

    void CRigidBody::spinning_friction(float value)
    {
        if (_spinning_friction != value)
        {
            _spinning_friction = value;
            set_modified("spinning_friction");
        }
    }

    float CRigidBody::linear_damping() const
    {
        return _linear_damping;
    }

    void CRigidBody::linear_damping(float value)
    {
        if (_linear_damping != value)
        {
            _linear_damping = value;
            set_modified("linear_damping");
        }
    }

    float CRigidBody::angular_damping() const
    {
        return _angular_damping;
    }

    void CRigidBody::angular_damping(float value)
    {
        if (_angular_damping != value)
        {
            _angular_damping = value;
            set_modified("angular_damping");
        }
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

    void CRigidBody::set_modified(const char* property_name)
    {
        _shared_data->set_modified(_node, this, property_name);
    }
}
