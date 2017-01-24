// CCapsuleCollider.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Math/Vec2.h>
#include "../../../include/Engine/Components/Physics/CCapsuleCollider.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"

SGE_REFLECT_TYPE(sge::CCapsuleCollider)
.property("radius", &CCapsuleCollider::radius, &CCapsuleCollider::radius)
.property("height", &CCapsuleCollider::height, &CCapsuleCollider::height);

namespace sge
{
    struct CCapsuleCollider::Data
    {
        ///////////////////
        ///   Methods   ///
    public:

        void to_archive(ArchiveWriter& writer) const
        {
            writer.object_member("s", shape);
        }

        void from_archive(ArchiveReader& reader)
        {
            reader.object_member("s", shape);
        }

        //////////////////
        ///   Fields   ///
    public:

        Vec2 shape = {1, 1};
    };

    CCapsuleCollider::CCapsuleCollider(ProcessingFrame& pframe, EntityId entity, Data& data)
        : TComponentInterface<sge::CCapsuleCollider>(pframe, entity),
        _data(&data)
    {
    }

    void CCapsuleCollider::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CCapsuleCollider, Data>>());
    }

    float CCapsuleCollider::radius() const
    {
        return _data->shape.x();
    }

    void CCapsuleCollider::radius(float value)
    {
        if (value != radius())
        {
            _data->shape.x(value);
            apply_component_modified_tag();
        }
    }

    float CCapsuleCollider::height() const
    {
        return _data->shape.y();
    }

    void CCapsuleCollider::height(float value)
    {
        if (value != height())
        {
            _data->shape.y(value);
            apply_component_modified_tag();
        }
    }
}
