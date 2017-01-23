// CVelocity.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Physics/CVelocity.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"

SGE_REFLECT_TYPE(sge::CVelocity)
.property("linear_velocity", &CVelocity::linear_velocity, &CVelocity::linear_velocity)
.property("angular_velocity", &CVelocity::angular_velocity, &CVelocity::angular_velocity);

namespace sge
{
    CVelocity::CVelocity(ProcessingFrame& pframe, EntityId entity, Data& data)
        : TComponentInterface<sge::CVelocity>(pframe, entity),
        _data(&data)
    {
    }

    void CVelocity::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CVelocity, Data>>());
    }

    Vec3 CVelocity::linear_velocity() const
    {
        return _data->linear_velocity;
    }

    void CVelocity::linear_velocity(const Vec3& value)
    {
        checked_setter(value, _data->linear_velocity);
    }

    Vec3 CVelocity::angular_velocity() const
    {
        return _data->angular_velocity;
    }

    void CVelocity::angular_velocity(const Vec3& value)
    {
        checked_setter(value, _data->angular_velocity);
    }

    CVelocity::Data CVelocity::data() const
    {
        return *_data;
    }

    void CVelocity::data(const Data& value)
    {
        checked_setter(value, *_data);
    }

    void CVelocity::Data::to_archive(ArchiveWriter& writer) const
    {
        writer.object_member("lin_v", linear_velocity);
        writer.object_member("ang_v", angular_velocity);
    }

    void CVelocity::Data::from_archive(ArchiveReader& reader)
    {
        reader.object_member("lin_v", linear_velocity);
        reader.object_member("ang_v", angular_velocity);
    }

    bool CVelocity::Data::operator==(const Data& rhs) const
    {
        return this->linear_velocity == rhs.linear_velocity && this->angular_velocity == rhs.angular_velocity;
    }

    bool CVelocity::Data::operator!=(const Data& rhs) const
    {
        return !(*this == rhs);
    }
}
