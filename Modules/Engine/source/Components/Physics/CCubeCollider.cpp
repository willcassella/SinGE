// CCubeCollider.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Physics/CCubeCollider.h"
#include "../../../include/Engine/Component.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"

SGE_REFLECT_TYPE(sge::CCubeCollider)
.property("width", &CCubeCollider::width, &CCubeCollider::width)
.property("height", &CCubeCollider::height, &CCubeCollider::height);

namespace sge
{
    struct CCubeCollider::Data
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

    CCubeCollider::CCubeCollider(ProcessingFrame& pframe, EntityId entity, Data& data)
        : TComponentInterface<sge::CCubeCollider>(pframe, entity),
        _data(&data)
    {
    }

    void CCubeCollider::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CCubeCollider, Data>>());
    }

    float CCubeCollider::width() const
    {
        return _data->shape.x();
    }

    void CCubeCollider::width(float value)
    {
        if (width() != value)
        {
            _data->shape.x(value);
            apply_component_modified_tag();
        }
    }

    float CCubeCollider::height() const
    {
        return _data->shape.y();
    }

    void CCubeCollider::height(float value)
    {
        if (height() != value)
        {
            _data->shape.y(value);
            apply_component_modified_tag();
        }
    }
}
