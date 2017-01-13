// CSpotLight.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CSpotlight.h"
#include "../../../include/Engine/Scene.h"

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

	CSpotlight::CSpotlight(ProcessingFrame& pframe, EntityId entity, Data& data)
		: TComponentInterface<CSpotlight>(pframe, entity),
		_data(&data)
	{
	}

	void CSpotlight::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CSpotlight, Data>>());
	}

	CSpotlight::Shape CSpotlight::shape() const
	{
		return _data->shape;
	}

	void CSpotlight::shape(Shape shape)
	{
		_data->shape = shape;
		apply_component_modified_tag();
	}

	void CSpotlight::set_cone(float radius)
	{
		_data->shape = Shape::CONE;
		_data->cone_radius = radius;
		apply_component_modified_tag();
	}

	void CSpotlight::set_frustum(float width, float height)
	{
		_data->shape = Shape::FRUSTUM;
		_data->frustum_width = width;
		_data->frustum_height = height;
		apply_component_modified_tag();
	}

	float CSpotlight::distance() const
	{
		return _data->distance;
	}

	void CSpotlight::distance(float value)
	{
		_data->distance = value;
		apply_component_modified_tag();
	}

	float CSpotlight::intensity() const
	{
		return _data->intensity;
	}

	void CSpotlight::intensity(float value)
	{
		_data->intensity = value;
		apply_component_modified_tag();
	}

	float CSpotlight::cone_radius() const
	{
		return _data->cone_radius;
	}

	void CSpotlight::cone_radius(float value)
	{
		_data->cone_radius = value;
		apply_component_modified_tag();
	}

	float CSpotlight::frustum_width() const
	{
		return _data->frustum_width;
	}

	void CSpotlight::frustum_width(float value)
	{
		_data->frustum_width = value;
		apply_component_modified_tag();
	}

	float CSpotlight::frustum_height() const
	{
		return _data->frustum_height;
	}

	void CSpotlight::frustum_height(float value)
	{
		_data->frustum_height = value;
		apply_component_modified_tag();
	}
}
