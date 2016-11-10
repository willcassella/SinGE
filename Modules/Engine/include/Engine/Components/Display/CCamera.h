// CCamera.h
#pragma once

#include <Core/Math/Mat4.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CPerspectiveCamera : TComponentInterface<CPerspectiveCamera>
	{
		SGE_REFLECTED_TYPE;
		struct Data;

		////////////////////////
		///   Constructors   ///
	public:

		CPerspectiveCamera(ProcessingFrame& pframe, EntityId entity, Data& data);

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

		Angle h_fov() const;

		void h_fov(Angle angle);

		float z_min() const;

		void z_min(float zMin);

		float z_max() const;

		void z_max(float zMax);

		Mat4 get_projection_matrix(float screenRatio) const;

		//////////////////
		///   Fields   ///
	private:

		Data* _data;
	};
}
