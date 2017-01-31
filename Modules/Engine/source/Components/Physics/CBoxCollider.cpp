// CBoxCollider.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Physics/CBoxCollider.h"
#include "../../../include/Engine/Component.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"

SGE_REFLECT_TYPE(sge::CBoxCollider)
.property("width", &CBoxCollider::width, &CBoxCollider::width)
.property("height", &CBoxCollider::height, &CBoxCollider::height)
.property("depth", &CBoxCollider::depth, &CBoxCollider::depth)
.property("shape", &CBoxCollider::shape, &CBoxCollider::shape, PF_EDITOR_HIDDEN);

namespace sge
{
    struct CBoxCollider::Data
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

        Vec3 shape = {1, 1, 1};
    };

    void CBoxCollider::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CBoxCollider, Data>>());
    }

    void CBoxCollider::reset(Data& data)
    {
        _data = &data;
    }

    float CBoxCollider::width() const
    {
        return _data->shape.x();
    }

    void CBoxCollider::width(float value)
    {
        if (width() != value)
        {
            _data->shape.x(value);
            set_modified();
        }
    }

    float CBoxCollider::height() const
    {
        return _data->shape.y();
    }

    void CBoxCollider::height(float value)
    {
        if (height() != value)
        {
            _data->shape.y(value);
            set_modified();
        }
    }

    float CBoxCollider::depth() const
    {
        return _data->shape.z();
    }

    void CBoxCollider::depth(float value)
    {
        if (depth() != value)
        {
            _data->shape.z(value);
            set_modified();
        }
    }

    Vec3 CBoxCollider::shape() const
    {
        return _data->shape;
    }

    void CBoxCollider::shape(const Vec3& value)
    {
        checked_setter(value, _data->shape);
    }
}
