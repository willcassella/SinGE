// CCamera.h
#pragma once

#include <Core/Math/Mat4.h>
#include "../../Component.h"

namespace sge
{
	class SGE_ENGINE_API CPerspectiveCamera final : public TComponentInterface<CPerspectiveCamera>
	{
	public:

		SGE_REFLECTED_TYPE;
		struct Data;

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

        void reset(Data& data);

		float h_fov() const;

		void h_fov(float angle);

		float z_min() const;

		void z_min(float zMin);

		float z_max() const;

		void z_max(float zMax);

		Mat4 get_projection_matrix(float screenRatio) const;

		//////////////////
		///   Fields   ///
	private:

		Data* _data = nullptr;
	};
}
