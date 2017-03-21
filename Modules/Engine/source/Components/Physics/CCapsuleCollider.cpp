// CCapsuleCollider.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Math/Vec2.h>
#include "../../../include/Engine/Components/Physics/CCapsuleCollider.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CCapsuleCollider)
.property("radius", &CCapsuleCollider::radius, &CCapsuleCollider::radius)
.property("height", &CCapsuleCollider::height, &CCapsuleCollider::height);

namespace sge
{
    struct CCapsuleCollider::SharedData : CSharedData<CCapsuleCollider>
    {
    };

	CCapsuleCollider::CCapsuleCollider(NodeId node, SharedData& shared_data)
		: _node(node),
		_shared_data(&shared_data)
	{
	}

    void CCapsuleCollider::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CCapsuleCollider, SharedData>>());
    }

	void CCapsuleCollider::to_archive(ArchiveWriter& writer) const
	{
		writer.object_member("s", _shape);
	}

	void CCapsuleCollider::from_archive(ArchiveReader& reader)
	{
		reader.object_member("s", _shape);
	}

	NodeId CCapsuleCollider::node() const
	{
		return _node;
	}

    float CCapsuleCollider::radius() const
    {
        return _shape.x();
    }

    void CCapsuleCollider::radius(float value)
    {
        if (value != radius())
        {
            _shape.x(value);
            set_modified("radius");
        }
    }

    float CCapsuleCollider::height() const
    {
        return _shape.y();
    }

    void CCapsuleCollider::height(float value)
    {
        if (value != height())
        {
            _shape.y(value);
            set_modified("height");
        }
    }

	void CCapsuleCollider::set_modified(const char* property_name)
	{
		_shared_data->set_modified(_node, this, property_name);
	}
}
