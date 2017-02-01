// CSpotLight.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CSpotlight.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"

SGE_REFLECT_TYPE(sge::CSpotlight)
.property("cone_radius", &CSpotlight::cone_radius, &CSpotlight::cone_radius)
.property("frustum_width", &CSpotlight::frustum_width, &CSpotlight::frustum_width)
.property("frustum_height", &CSpotlight::frustum_height, &CSpotlight::frustum_height);

namespace sge
{
	struct CSpotlight::Data
	{
		///////////////////
		///   Methods   ///
	public:

		void to_archive(ArchiveWriter& writer) const
		{
			if (shape == Shape::CONE)
			{
				writer.push_object_member("cone");
				writer.object_member("radius", cone_radius);
				writer.pop();
			}
			else if (shape == Shape::FRUSTUM)
			{
				writer.push_object_member("frustum");
				writer.object_member("width", frustum_width);
				writer.object_member("height", frustum_height);
				writer.pop();
			}

			writer.object_member("distance", distance);
			writer.object_member("intensity", intensity);
		}

		void from_archive(ArchiveReader& reader)
		{
			if (reader.pull_object_member("cone"))
			{
				shape = Shape::CONE;
				reader.object_member("radius", cone_radius);
				reader.pop();
			}
			else if (reader.pull_object_member("frustum"))
			{
				shape = Shape::FRUSTUM;
				reader.object_member("width", frustum_width);
				reader.object_member("height", frustum_height);
				reader.pop();
			}
		}

		//////////////////
		///   Fields   ///
	public:

		Shape shape = Shape::CONE;
		float cone_radius = 1.f;
		float frustum_width = 1.f;
		float frustum_height = 1.f;
		float distance = 1.f;
		float intensity = 1.f;
	};

	void CSpotlight::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CSpotlight, Data>>());
	}

    void CSpotlight::reset(Data& data)
    {
        _data = &data;
    }

    CSpotlight::Shape CSpotlight::shape() const
	{
		return _data->shape;
	}

	void CSpotlight::shape(Shape shape)
	{
        checked_setter(shape, _data->shape);
	}

	void CSpotlight::set_cone(float radius)
	{
        checked_setter(Shape::CONE, _data->shape);
        checked_setter(radius, _data->cone_radius);
	}

	void CSpotlight::set_frustum(float width, float height)
	{
        checked_setter(Shape::FRUSTUM, _data->shape);
        checked_setter(width, _data->frustum_width);
        checked_setter(height, _data->frustum_height);
	}

	float CSpotlight::distance() const
	{
		return _data->distance;
	}

	void CSpotlight::distance(float value)
	{
        checked_setter(value, _data->distance);
	}

	float CSpotlight::intensity() const
	{
		return _data->intensity;
	}

	void CSpotlight::intensity(float value)
	{
        checked_setter(value, _data->intensity);
	}

	float CSpotlight::cone_radius() const
	{
		return _data->cone_radius;
	}

	void CSpotlight::cone_radius(float value)
	{
        checked_setter(value, _data->cone_radius);
	}

	float CSpotlight::frustum_width() const
	{
		return _data->frustum_width;
	}

	void CSpotlight::frustum_width(float value)
	{
        checked_setter(value, _data->frustum_width);
	}

	float CSpotlight::frustum_height() const
	{
		return _data->frustum_height;
	}

	void CSpotlight::frustum_height(float value)
	{
        checked_setter(value, _data->frustum_height);
	}
}
