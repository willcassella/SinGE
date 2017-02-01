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

    void CCapsuleCollider::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CCapsuleCollider, Data>>());
    }

    void CCapsuleCollider::reset(Data& data)
    {
        _data = &data;
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
            set_modified();
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
            set_modified();
        }
    }
}
