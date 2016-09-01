// CubeCollider.h
#pragma once

#include <Core/Math/Vec2.h>
#include "../../config.h"

namespace sge
{
	struct SGE_ENGINE_API CCubeCollider 
	{
		SGE_REFLECTED_TYPE;
		
		////////////////
		///   Tags   ///
	public:

		struct SGE_ENGINE_API TShapeChanged 
		{
			SGE_REFLECTED_TYPE;
		};

		////////////////////
		////   Methods   ///
	public:

		Vec2 get_shape() const
		{
			return _shape;
		}
		
		void set_shape(struct Frame& frame, Vec2 shape)
		{
			_shape = shape;
		}
		
		////////////////
		///   Data   ///
	private:

		Vec2 _shape;
	};
}
