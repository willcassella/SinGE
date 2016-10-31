// CCamera.h
#pragma once

#include <Core/Math/Mat4.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CPerspectiveCamera : BasicComponent<CPerspectiveCamera>
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		CPerspectiveCamera();

		///////////////////
		///   Methods   ///
	public:

		Mat4 get_projection_matrix(float screenRatio) const
		{
			return Mat4::perspective_projection_hfov(h_fov, screenRatio, z_min, z_max);
		}

		//////////////////
		///   Fields   ///
	public:

		Angle h_fov;
		float z_min;
		float z_max;
	};

	struct SGE_ENGINE_API COrthographicCamera : BasicComponent<CPerspectiveCamera>
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		COrthographicCamera();

		///////////////////
		///   Fields   ///
	public:

		float scale;
		float z_min;
		float z_max;
	};
}
