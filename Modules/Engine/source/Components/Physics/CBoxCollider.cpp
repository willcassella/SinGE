// CBoxCollider.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Physics/CBoxCollider.h"
#include "../../../include/Engine/Component.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"

SGE_REFLECT_TYPE(sge::CBoxCollider)
.property("width", &CBoxCollider::width, &CBoxCollider::width)
.property("height", &CBoxCollider::height, &CBoxCollider::height)
.property("depth", &CBoxCollider::depth, &CBoxCollider::depth)
.property("shape", &CBoxCollider::shape, &CBoxCollider::shape, PF_EDITOR_HIDDEN);

namespace sge
{
	struct CBoxCollider::SharedData : public CSharedData<CBoxCollider>
	{
	};

	CBoxCollider::CBoxCollider(NodeId node, SharedData& shared_data)
		: _node(node),
		_shared_data(&shared_data)
	{
	}

    void CBoxCollider::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CBoxCollider, SharedData>>());
    }

	void CBoxCollider::to_archive(ArchiveWriter& writer) const
	{
		writer.object_member("s", _shape);
	}

	void CBoxCollider::from_archive(ArchiveReader& reader)
	{
		reader.object_member("s", _shape);
	}

    float CBoxCollider::width() const
    {
        return _shape.x();
    }

    void CBoxCollider::width(float value)
    {
        if (width() != value)
        {
            _shape.x(value);
            set_modified();
        }
    }

    float CBoxCollider::height() const
    {
        return _shape.y();
    }

    void CBoxCollider::height(float value)
    {
        if (height() != value)
        {
            _shape.y(value);
            set_modified();
        }
    }

    float CBoxCollider::depth() const
    {
        return _shape.z();
    }

    void CBoxCollider::depth(float value)
    {
        if (depth() != value)
        {
            _shape.z(value);
            set_modified();
        }
    }

    Vec3 CBoxCollider::shape() const
    {
        return _shape;
    }

    void CBoxCollider::shape(const Vec3& value)
    {
		if (_shape != value)
		{
			_shape = value;
			set_modified();
		}
    }

	void CBoxCollider::set_modified()
	{
		_shared_data->set_modified(_node, this, "shape");
	}
}
