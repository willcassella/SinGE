// CCamera.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CCamera.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"

SGE_REFLECT_TYPE(sge::CPerspectiveCamera)
.property("h_fov", &CPerspectiveCamera::h_fov, &CPerspectiveCamera::h_fov)
.property("z_min", &CPerspectiveCamera::z_min, &CPerspectiveCamera::z_min)
.property("z_max", &CPerspectiveCamera::z_max, &CPerspectiveCamera::z_max);

namespace sge
{
	struct CPerspectiveCamera::SharedData : CSharedData<CPerspectiveCamera>
	{
	};

	CPerspectiveCamera::CPerspectiveCamera(NodeId node, SharedData& shared_data)
		: _node(node),
		_shared_data(&shared_data)
	{
	}

	void CPerspectiveCamera::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CPerspectiveCamera, SharedData>>());
	}

	void CPerspectiveCamera::to_archive(ArchiveWriter& writer) const
	{
		writer.object_member("h_fov", _h_fov);
		writer.object_member("z_min", _z_min);
		writer.object_member("z_max", _z_max);
	}

	void CPerspectiveCamera::from_archive(ArchiveReader& reader)
	{
		reader.object_member("h_fov", _h_fov);
		reader.object_member("z_min", _z_min);
		reader.object_member("z_max", _z_max);
	}

    float CPerspectiveCamera::h_fov() const
	{
		return _h_fov.degrees();
	}

	void CPerspectiveCamera::h_fov(float angle)
	{
		const auto deg = degrees(angle);
		if (deg != _h_fov)
		{
			_h_fov = deg;
			set_modified("h_fov");
		}
	}

	float CPerspectiveCamera::z_min() const
	{
		return _z_min;
	}

	void CPerspectiveCamera::z_min(float value)
	{
		if (value != _z_min)
		{
			_z_min = value;
			set_modified("z_min");
		}
	}

	float CPerspectiveCamera::z_max() const
	{
		return _z_max;
	}

	void CPerspectiveCamera::z_max(float value)
	{
		if (value != _z_max)
		{
			_z_max = value;
			set_modified("z_max");
		}
	}

	Mat4 CPerspectiveCamera::get_projection_matrix(float screen_ratio) const
	{
		return Mat4::perspective_projection_hfov(_h_fov, screen_ratio, _z_min, _z_max);
	}

	void CPerspectiveCamera::set_modified(const char* prop_name)
	{

	}
}
