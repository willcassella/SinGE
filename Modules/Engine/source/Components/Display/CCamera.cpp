// CCamera.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CCamera.h"

SGE_REFLECT_TYPE(sge::CPerspectiveCamera)
.field_property("h_fov", &CPerspectiveCamera::h_fov)
.field_property("z_min", &CPerspectiveCamera::z_min)
.field_property("z_max", &CPerspectiveCamera::z_max);

SGE_REFLECT_TYPE(sge::COrthographicCamera)
.field_property("scale", &COrthographicCamera::scale);

namespace sge
{
	CPerspectiveCamera::CPerspectiveCamera()
		: h_fov(degrees(90.f)),
		z_min(0.1),
		z_max(100)
	{
	}

	COrthographicCamera::COrthographicCamera()
		: scale(10),
		z_min(0.1),
		z_max(100)
	{
	}
}
