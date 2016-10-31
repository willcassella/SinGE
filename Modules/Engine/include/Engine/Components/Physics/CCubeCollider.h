// CCubeCollider.h
#pragma once

#include <Core/Math/Vec2.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CCubeCollider
	{
		SGE_REFLECTED_TYPE;

		////////////////
		///   Tags   ///
	public:

		struct SGE_ENGINE_API FShapeChanged
		{
			SGE_REFLECTED_TYPE;
		};

		////////////////////
		////   Methods   ///
	public:

		Vec2 shape() const
		{
			return _shape;
		}

		void shape(Vec2 shape)
		{
			_shape = shape;
		}

		static void set_shape(TComponentInstance<CCubeCollider> self, Frame& frame, Vec2 shape);

		//////////////////
		///   Fields   ///
	private:

		Vec2 _shape;
	};
}
