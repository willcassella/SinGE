// CCamera.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CCamera.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CPerspectiveCamera)
.property("h_fov", &CPerspectiveCamera::h_fov, &CPerspectiveCamera::h_fov)
.property("z_min", &CPerspectiveCamera::z_min, &CPerspectiveCamera::z_min)
.property("z_max", &CPerspectiveCamera::z_max, &CPerspectiveCamera::z_max);

namespace sge
{
	struct CPerspectiveCamera::Data
	{
		////////////////////////
		///   Constructors   ///
	public:

		Data()
			: h_fov(degrees(90.f)),
			z_min(0.1f),
			z_max(100.f)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		void to_archive(ArchiveWriter& writer) const
		{
			writer.push_object_member("z_min", z_min);
			writer.push_object_member("z_max", z_max);
		}

		void from_archive(const ArchiveReader& reader)
		{
			reader.pull_object_member("z_min", z_min);
			reader.pull_object_member("z_max", z_max);
		}

		//////////////////
		///   Fields   ///
	public:

		Angle h_fov;
		float z_min;
		float z_max;
	};

	CPerspectiveCamera::CPerspectiveCamera(ProcessingFrame& pframe, EntityId entity, Data& data)
		: TComponentInterface<sge::CPerspectiveCamera>(pframe, entity),
		_data(&data)
	{
	}

	void CPerspectiveCamera::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CPerspectiveCamera, Data>>());
	}

	float CPerspectiveCamera::h_fov() const
	{
		return _data->h_fov.degrees();
	}

	void CPerspectiveCamera::h_fov(float angle)
	{
		_data->h_fov = degrees(angle);
	}

	float CPerspectiveCamera::z_min() const
	{
		return _data->z_min;
	}

	void CPerspectiveCamera::z_min(float zMin)
	{
		_data->z_min = zMin;
	}

	float CPerspectiveCamera::z_max() const
	{
		return _data->z_max;
	}

	void CPerspectiveCamera::z_max(float zMax)
	{
		_data->z_max = zMax;
	}

	Mat4 CPerspectiveCamera::get_projection_matrix(float screenRatio) const
	{
		return Mat4::perspective_projection_hfov(_data->h_fov, screenRatio, _data->z_min, _data->z_max);
	}
}
