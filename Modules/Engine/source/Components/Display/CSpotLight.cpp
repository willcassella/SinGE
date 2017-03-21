// CSpotLight.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CSpotlight.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"

SGE_REFLECT_TYPE(sge::CSpotlight)
.property("cone_radius", &CSpotlight::cone_radius, &CSpotlight::cone_radius)
.property("frustum_width", &CSpotlight::frustum_width, &CSpotlight::frustum_width)
.property("frustum_height", &CSpotlight::frustum_height, &CSpotlight::frustum_height);

namespace sge
{
	struct CSpotlight::SharedData : CSharedData<CSpotlight>
	{
	};

	CSpotlight::CSpotlight(NodeId node, SharedData& shared_data)
		: _node(node),
		_shared_data(&shared_data)
	{
	}

	void CSpotlight::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CSpotlight, SharedData>>());
	}

	void CSpotlight::to_archive(ArchiveWriter& writer) const
	{
		if (_shape == Shape::CONE)
		{
			writer.push_object_member("cone");
			writer.object_member("radius", _cone_radius);
			writer.pop();
		}
		else if (_shape == Shape::FRUSTUM)
		{
			writer.push_object_member("frustum");
			writer.object_member("width", _frustum_width);
			writer.object_member("height", _frustum_height);
			writer.pop();
		}

		writer.object_member("distance", _distance);
		writer.object_member("intensity", _intensity);
	}

	void CSpotlight::from_archive(ArchiveReader& reader)
	{
		if (reader.pull_object_member("cone"))
		{
			_shape = Shape::CONE;
			reader.object_member("radius", _cone_radius);
			reader.pop();
		}
		else if (reader.pull_object_member("frustum"))
		{
			_shape = Shape::FRUSTUM;
			reader.object_member("width", _frustum_width);
			reader.object_member("height", _frustum_height);
			reader.pop();
		}
	}

	NodeId CSpotlight::node() const
	{
		return _node;
	}

    CSpotlight::Shape CSpotlight::shape() const
	{
		return _shape;
	}

	void CSpotlight::shape(Shape shape)
	{
		if (_shape != shape)
		{
			_shape = shape;
			set_modified("shape");
		}
	}

	void CSpotlight::set_cone(float radius)
	{
		shape(Shape::CONE);
		cone_radius(radius);
	}

	void CSpotlight::set_frustum(float width, float height)
	{
		shape(Shape::FRUSTUM);
		frustum_width(width);
		frustum_height(height);
	}

	float CSpotlight::distance() const
	{
		return _distance;
	}

	void CSpotlight::distance(float value)
	{
		if (_distance != value)
		{
			_distance = value;
			set_modified("distance");
		}
	}

	float CSpotlight::intensity() const
	{
		return _intensity;
	}

	void CSpotlight::intensity(float value)
	{
		if (_intensity != value)
		{
			_intensity = value;
			set_modified("intensity");
		}
	}

	float CSpotlight::cone_radius() const
	{
		return _cone_radius;
	}

	void CSpotlight::cone_radius(float value)
	{
		if (_cone_radius != value)
		{
			_cone_radius = value;
			set_modified("cone_radius");
		}
	}

	float CSpotlight::frustum_width() const
	{
		return _frustum_width;
	}

	void CSpotlight::frustum_width(float value)
	{
		if (_frustum_width != value)
		{
			_frustum_width = value;
			set_modified("frustum_width");
		}
	}

	float CSpotlight::frustum_height() const
	{
		return _frustum_height;
	}

	void CSpotlight::frustum_height(float value)
	{
		if (_frustum_height != value)
		{
			set_modified("frustum_height");
		}
	}

	void CSpotlight::set_modified(const char* property_name)
	{
		_shared_data->set_modified(_node, this, property_name);
	}
}
